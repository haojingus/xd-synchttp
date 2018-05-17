xd-synchttp - a http library for node
===========================

[![Build Status](https://travis-ci.org/haojingus/xd-synchttp.svg?branch=master)](https://travis-ci.org/haojingus/xd-synchttp)

This is a synchronous http library for node.js. __It supports GET/POST
request and focuses on high performance.

Install with:

    npm install xd-synchttp

## Usage Example

```js
var sync = require("xd-synchttp");

var response_get = "";
var response_post = "";
var timeout = 3;
var post_data = {"field1":"1","field2":"2"};
var url = "http://www.baidu.com";
try{
	response_get = sync.http_get(url,timeout);
	response_post = sync.http_post(url,post_data,timeout);
}
catch(err){
	console.log(err);
}
console.log(response_get);
console.log(response_post);
```

Note that the API is not support https.
### Promises

not supported


## API

### "http_get"

http_get(string,number);

`http_get` send a GET request

### "http_post"

http_post(string,object,number);

`http_post` send a POST request

## How to Contribute
- Open a pull request or an issue about what you want to implement / change. We're glad for any help!
 - Please be aware that we'll only accept fully tested code.

## Contributors

The original author of xd-synchttp is [Jing HAO](https://github.com/haojingus)

## License

[GPL](LICENSE)


