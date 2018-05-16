'use strict';
const synchttp = require('../vendor/linux-x86-amd64/binding');
let content = "";
try{
	content = synchttp.http_get("http://www.baidu.com",10);
	console.log(content);
	console.log("\nRecv Length:"+content.length);
}
catch(err)
{
	console.log(err);
}


