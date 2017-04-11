#!/usr/bin/env nodejs

var connect = require('connect');
var compression = require('compression');
var chprocess = require('child_process');
var url = require('url');

function makeFractal(parms, cb) {
    cmd = "./fractizer";
    argnames = [
        [ 'max_iters', 'i', ],
        [ 'escape_val', 'f', ],
        [ 'x_min', 'f', ],
        [ 'x_max', 'f', ],
        [ 'y_min', 'f', ],
        [ 'y_max', 'f', ], 
        [ 'x_pels', 'i' ],
        [ 'y_pels', 'i' ],
        [ 'x_tile', 'i' ],
        [ 'y_tile', 'i' ],
    ];
    args = argnames.map(function(n) { return parms[n[0]].toString(); });
    cmd += ' ' + args.join(' ');

    console.log(cmd);

    chprocess.exec(cmd, {maxBuffer: 1024 * 1024 * 64 }, function(err, stdout, stdeff) {
        if (err) console.log("ERROR: " + err);
        var res = {
            'params': {},
            'data': [],
        };
        var lines = stdout.split("\n");
        for (var i=0; i<lines.length; i++) {
            var line = lines[i];
            if (i === 0) {
                var chunks = line.split(/\s/);
                for (var j=0;j<chunks.length;j++) {
                    if (argnames[j][1] === 'i') {
                        res.params[argnames[j][0]] = parseInt(chunks[j]);
                    } else {
                        res.params[argnames[j][0]] = parseFloat(chunks[j]);
                    }
                }
            } else if (line === 'all_done') {
                return cb(null, res);
            } else {
                res.data.push(parseInt(line));
            }
        }
    });
}

function getParamsFromUrl(inurl) {
    console.log(inurl);
    var sp = url.parse(inurl).query;
    var chunks = sp.split(/&/);
    var res = {};
    for (var i=0; i< chunks.length; i++) {
        var nv = chunks[i].split(/=/);
        res[nv[0]] = nv[1];
    }
    return res;
}

var app = connect();
app.use(compression({ filter: function(req,res) { return true; } }));

app.use(function(req, res) {
    console.log('got request');
    var parms = getParamsFromUrl(req.url);
    var headers = {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': '*',
    };
    makeFractal(parms, function(ferr, fres) {
        if (ferr) {
            res.writeHead(500,headers);
            res.end(JSON.stringify({'ERROR': 'Something went wrong.'}));
            return;
        }
        res.writeHead(200,headers);
        res.end(JSON.stringify(fres));
    });
});

app.listen(5001);


