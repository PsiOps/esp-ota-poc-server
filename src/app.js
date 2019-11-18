'use strict';
const fs = require('fs');
const { spawn } = require( 'child_process' );
const AWS = require('aws-sdk');
const ak = "AKIA2TNS7VT7AY7C42VP";
const sk = "CCSZFc0bJTiG/2ukYT0FolJWTUzFD/igXi4plswb";
const s3 = new AWS.S3({
    accessKeyId: ak,
    secretAccessKey: sk
});
var express = require("express");
var bodyParser = require("body-parser");
var app = express();
//Here we are configuring express to use body-parser as middle-ware.
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

const bucketName = "esp-ota-binaries";

app.listen(80, () => {
 console.log("Server running on port 80");
});

app.get("/test", (req, res, next) => {
 res.json(["Tony","Lisa","Michael","Ginger","Food"]);
});

app.post('/compile/:robotId', (req,res) => {
    var robotId = req.params.robotId;
    console.log(`Compiling for robot with id ${robotId}`);
    var sketch = req.body.sketch;
    var dir  = `/builds/${robotId}`;
    var path = `${dir}/Sketch.ino`;
    !fs.existsSync(dir) && fs.mkdirSync(dir);
    fs.writeFile(path, sketch, function(err) {
        if(err) {return console.log(err);}
        console.log("The file was saved!");
        // arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 /builds/${robotId}/Sketch.ino`
        const compile = spawn( '/home/ubuntu/bin/arduino-cli', [ 'compile', '--fqbn', 'esp8266:esp8266:nodemcuv2', path ] );
        compile.stdout.on( 'data', data => {
            console.log( `stdout: ${data}` );
        } );
        compile.stderr.on( 'data', data => {
            console.log( `stderr: ${data}` );
        } );
        compile.on('error', err => {
            console.log(`Error thrown: ${err}`);
        });
        compile.on('close', code => {
            console.log( `child process exited with code ${code}`);
            if(code != 1) { 
                res.json({message: "Compilation failed"});
                return;
            }
            const localFileName = `/builds/${robotId}/Sketch.ino.esp8266.esp8266.nodemcuv2.bin`;
            const fileContent = fs.readFileSync(localFileName);
            const remoteFileName = `${robotId}-${new Date().getTime()}.bin`;
            const params = {
                Bucket: bucketName,
                Key: `binaries/${robotId}/${remoteFileName}`,
                Body: fileContent,
                ACL: 'public-read'
            };
            s3.upload(params, function(err, data) {
                if (err) { throw err; }
                console.log(`Binary file uploaded successfully. ${data.Location}`);
                const latestFileContent = new Buffer(remoteFileName);
                const latestParams  = {
                    Bucket: bucketName,
                    Key: `binaries/${robotId}/latest.txt`,
                    Body: latestFileContent,
                    ACL: 'public-read'    
                }
                s3.upload(latestParams, function(latestErr, latestData){
                    if (latestErr) { throw latestErr; }
                    console.log(`Latest txt file uploaded successfully. ${latestData.Location}`);
                    res.json({message: `Binary saved to ${latestData.Location}`});
                });
            });
        });
    }); 
});
