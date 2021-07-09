const express = require('express')
const app = express()
var path = require('path');

app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/index.html'));
})

app.listen(3000, function () {
  console.log('GBGO is available at http://localhost:3000')
})


app.use(express.static("public")) 