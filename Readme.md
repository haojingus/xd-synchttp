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

### Consolidation: It's time for celebration

Right now there are two great redis clients around and both have some advantages
above each other. We speak about ioredis and node_redis. So after talking to
each other about how we could improve in working together we (that is @luin and
@BridgeAR) decided to work towards a single library on the long run. But step by
step.

First of all, we want to split small parts of our libraries into others so that
we're both able to use the same code. Those libraries are going to be maintained
under the NodeRedis organization. This is going to reduce the maintenance
overhead, allows others to use the very same code, if they need it and it's way
easyer for others to contribute to both libraries.

We're very happy about this step towards working together as we both want to
give you the best redis experience possible.

If you want to join our cause by help maintaining something, please don't
hesitate to contact either one of us.
