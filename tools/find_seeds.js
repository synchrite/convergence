// Find, for each of the 40 stages, a layout that starts visibly broken and that the solver can fix
// with that stage's strips and rings. Writes the seed table into index.html between the SEEDS markers.
// Run: node tools/find_seeds.js easy|medium|hard   (needs Playwright with Chromium)
// Each mode holds the solver to that mode's strip angles, so no stage needs a turn the mode does not allow.
const {chromium}=require('playwright'),fs=require('fs'),path=require('path');
(async()=>{
  const file=path.resolve(__dirname,'..','index.html');
  const b=await chromium.launch();const p=await b.newPage();
  p.on('pageerror',e=>console.log('PAGE ERROR',e.message));
  await p.goto('file://'+file+'#level-1');await p.waitForTimeout(300);
  const mode=process.argv[2]||'easy';await p.evaluate(m=>window.__gen.setMode(m),mode);console.log('mode',mode);
  const from=+(process.argv[3]||1);   // optional: first stage to search; earlier ones keep their stored seeds
  const old=JSON.parse(fs.readFileSync(file,'utf8').split('/*SEEDS*/')[1].split('/*END-SEEDS*/')[0])[mode]||[];
  const seeds=[null];for(let i=1;i<from;i++)seeds.push(old[i]??null);
  for(let st=from;st<=40;st++){
    const r=await p.evaluate(async st=>{
      const G=window.__gen,V=window.__cv,c=G.cfg(st);
      for(let t=0;t<40;t++){
        G.genLevel(st,t);const e=V.evaluate();
        const broken=(e.wc>=c.conv*1.6||(c.pur!=null&&e.wp>=c.pur*1.3))&&(e.wc<(c.L===1?9:c.L===2?16:40)||c.L===4);
        if(!broken)continue;
        for(let a=0;a<2;a++){G.genLevel(st,t);await V.autoPlace(c.budget,false);const f=V.evaluate();
          if(f.ok)return{t,start:e.wc.toFixed(1),end:f.wc.toFixed(2),strips:V.strips.length,rings:JSON.stringify(G.rv)}}
      }
      return null;
    },st);
    console.log('stage',st,r?JSON.stringify(r):'NO SOLVABLE LAYOUT FOUND');
    seeds.push(r?r.t:null);
  }
  await b.close();
  let s=fs.readFileSync(file,'utf8');const a=s.indexOf('/*SEEDS*/'),z=s.indexOf('/*END-SEEDS*/');
  const all=JSON.parse(s.slice(a+9,z));all[mode]=seeds;
  s=s.slice(0,a)+'/*SEEDS*/'+JSON.stringify(all)+s.slice(z);fs.writeFileSync(file,s);
  console.log('seeds written:',JSON.stringify(seeds));
})();
