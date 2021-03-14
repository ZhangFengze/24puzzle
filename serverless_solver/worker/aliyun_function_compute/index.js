const child_process = require('child_process');
const getRawBody = require('raw-body');

exports.handler = (req, resp, context) => {
    getRawBody(req, function (err, body) {
        if (err) {
            console.log(err)
            resp.send("error")
            return
        }

        let child = child_process.exec("./worker",
            (err, stdout, stderr) => {
                if (err) {
                    console.log(err)
                    console.log(stderr)
                    resp.send("error")
                    return
                }
                resp.send(stdout)
            })
        child.stdin.end(body)
    });
}