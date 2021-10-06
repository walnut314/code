#!/usr/bin/env node

// connect in browser at: http://localhost:3000/
// or just do: curl http://localhost:3000/

//server.js
const http = require('http'),
      url = require('url'),
 
makeServer = function (request,response){
   let path = url.parse(request.url).pathname;
   console.log(path);
   if(path === '/'){
      response.writeHead(200,{'Content-Type':'text/plain'});
      response.write('yo sup');
   }
   else if(path === '/about'){
     response.writeHead(200,{'Content-Type':'text/plain'});
     response.write('About time!!');
   }
   else if(path === '/blog'){
     response.writeHead(200,{'Content-Type':'text/plain'});
     response.write('Blog page');
   }
   else{
     response.writeHead(404,{'Content-Type':'text/plain'});
     response.write('Error page');
   }
   response.end();
 },
server = http.createServer(makeServer);
server.listen(3000,()=>{
 console.log('Node server created at port 3000');
});


