const cluster = require('cluster');
const http = require('http');
const child_process = require('child_process');
const numCPUs = require('os').cpus().length;

if (cluster.isMaster) {
  console.log(`Master ${process.pid} is running`);

  for (let i = 0; i < numCPUs; i++) {
    cluster.fork();
  }

  cluster.on('exit', (worker, code, signal) => {
    console.log(`worker ${worker.process.pid} died`);
  });
} else {
  http.createServer(function (req, res) {
    let received = ''
    req.on("data", (data) => received += data)
    req.on("end", () => {
      let child = child_process.exec("worker.exe",
        (err, stdout, stderr) => {
          if (err) {
            res.end("error")
            return
          }
          res.end(stdout)
        })
      child.stdin.end(received)
    })
  }).listen(8088);

  console.log(`Worker ${process.pid} started`);
}