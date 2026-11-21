// Find, for each of the 50 stages, a layout that starts visibly broken and that the solver can fix
// with that stage's strips and rings. Writes the seed table into archive/inside-the-tube/index.html between the SEEDS markers.
// Run: node tools/find_seeds.js easy|medium|hard   (needs Playwright with Chromium)
// Each mode holds the solver to that mode's strip angles, so no stage needs a turn the mode does not allow.
// A layout is kept only if it can be solved with the stage's strips AND cannot be solved with one strip fewer,
// so the rings alone are never enough where strips are provided.
const {chromium}=require('playwright'),fs=require('fs'),path=require('path');
(async()=>{
  const file=path.resolve(__dirname,'..','archive','inside-the-tube','index.html');
  const b=await chromium.launch();const p=await b.newPage();
  p.on('pageerror',e=>console.log('PAGE ERROR',e.message));
  await p.goto('file://'+file+'#level-1');await p.waitForTimeout(300);
  const mode=process.argv[2]||'easy';await p.evaluate(m=>window.__gen.setMode(m),mode);console.log('mode',mode);
  const from=+(process.argv[3]||1);   // optional: first stage to search; earlier ones keep their stored seeds
  const old=JSON.parse(fs.readFileSync(file,'utf8').split('/*SEEDS*/')[1].split('/*END-SEEDS*/')[0])[mode]||[];
  const to=+(process.argv[4]||50),need=process.argv[5]!=='any';   // 'any' as a fifth argument skips the test that the strips are needed     // optional: last stage to search
  const seeds=[null];for(let i=1;i<from;i++)seeds.push(old[i]??null);
  for(let st=from;st<=to;st++){
    const r=await p.evaluate(async([st,need])=>{
      const G=window.__gen,V=window.__cv,c=G.cfg(st);let ringsEnough=0,unsolved=0,fallback=null;
      for(let t=0;t<100;t++){
        G.genLevel(st,t);const e=V.evaluate();
        const broken=(e.wc>=c.conv*1.6||(c.pur!=null&&e.wp>=c.pur*1.3))&&e.wc<(c.L===1?9:c.L===2?16:c.L===3?40:30);
        if(!broken)continue;
        let solved=null;
        for(let a=0;a<2&&!solved;a++){G.genLevel(st,t);await V.autoPlace(c.budget,false);const f=V.evaluate();
          if(f.ok)solved={t,start:e.wc.toFixed(1),end:f.wc.toFixed(2)}}
        if(!solved){unsolved++;continue}
        if(!(c.n>0&&c.rings.length>0&&need))return solved;
        // Rings and yoke alone must never be enough where strips are provided.
        const can=async K=>{for(let a=0;a<2;a++){G.genLevel(st,t);await V.autoPlace(K,false);if(V.evaluate().ok)return true}return false};
        if(await can(0)){ringsEnough++;continue}
        // Better still: it should need every strip the level gives. Keep the first layout that does;
        // failing that, after 20 tries, the first one that at least needs a strip.
        if(c.budget===1||!(await can(c.budget-1)))return{...solved,needs:'all '+c.budget+' strips',ringsEnough,unsolved};
        if(!fallback)fallback={...solved,needs:'at least one strip'};
        if(t>=20)break;
      }
      return fallback?{...fallback,ringsEnough,unsolved}:{none:true,ringsEnough,unsolved};
    },[st,need]);
    console.log('stage',st,r&&!r.none?JSON.stringify(r):'NO LAYOUT FOUND '+JSON.stringify(r));
    seeds.push(r&&!r.none?r.t:null);
  }
  for(let i=to+1;i<=50;i++)seeds.push(old[i]??null);
  await b.close();
  let s=fs.readFileSync(file,'utf8');const a=s.indexOf('/*SEEDS*/'),z=s.indexOf('/*END-SEEDS*/');
  const all=JSON.parse(s.slice(a+9,z));all[mode]=seeds;
  s=s.slice(0,a)+'/*SEEDS*/'+JSON.stringify(all)+s.slice(z);fs.writeFileSync(file,s);
  console.log('seeds written:',JSON.stringify(seeds));
})();
