#!/usr/bin/env nodejs

var connect = require('connect');
var compression = require('compression');
var chprocess = require('child_process');
var url = require('url');
var fs = require('fs');
var request = require('request');
var submit_count = 0;

var local_debug = false;
var slaves = [ 'slave0', 'slave1', 'slave2', 'slave3', 'slave4' ,'slave5' ];
if (local_debug) {
    slaves = [ 'localhost', ];
}

var app = connect();

app.use(compression({
    filter: function(req, res) {
        if (req.url.match(/render\//)) return true;
        return false;
    }
}));

app.use(function(req, res) {
    var headers = {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': '*',
    };

    if (req.url.match(/(\/$|htm$|html$)/)) {
        console.log('get client html');
        fs.readFile('webclient.html', function(err, data) {
            res.writeHead(200);
            res.end(data);
        });
    } else if (req.url.match(/\.js$/)) {
        console.log('get client js');
        fs.readFile('webclient.js', function(err, data) {
            res.writeHead(200);
            res.end(data);
        });
    } else if (req.url.match(/render\//)) {
        console.log('render request');
        var oldpurl = url.parse(req.url);

        var slave_idx = submit_count % slaves.length;
        var slave = slaves[slave_idx];
        submit_count += 1;
        var newurl  = 'http://' + slave + ':5001/?' +
                      oldpurl.query;
        console.log(newurl);
        request(newurl,function(perr,pres,pbody) {
            console.log('render response');
            if (perr) {
                console.log('render error');
                console.log(perr);
                res.writeHead(500,headers);
                res.end('');
                return;
            }
            res.writeHead(200,headers);
            res.end(pbody);
        });
    } else {
        console.log('ruh roh');
        res.writeHead(500,headers);
        res.end(JSON.stringify({'ERROR': 'Something went wrong.'}));
    }
});

app.listen(6891);


