var express = require('express');
var app = express();

//CORS middleware
var allowCrossDomain = function(req, res, next) {
    res.header('Access-Control-Allow-Origin', 'localhost:9999');
    res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
    res.header('Access-Control-Allow-Headers', 'Content-Type');

    next();
}

app.use(allowCrossDomain);

app.get('/', function (req, res) {
  // res.send('Hello World!');
  res.header('Access-Control-Allow-Origin', 'example.com');
  res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
  res.header('Access-Control-Allow-Headers', 'Content-Type');
  res.sendFile("index.html", { root: __dirname });
});

app.use(express.static(__dirname));

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});
