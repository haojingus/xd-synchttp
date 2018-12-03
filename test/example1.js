'use strict';
const synchttp = require('../src/build/Release/binding');
let content = "";
try{
	//old function
	//content = synchttp.http_get("http://www.baidu.com",10);
	//new function
	content = synchttp.sync_http("http://cdn2.jianshu.io/assets/web/nav-logo-4c7bbafe27adc892f3046e6978459bac.png","get",{},10);
	//console.log(content);
	console.log("\nRecv Length:"+content.length);
}
catch(err)
{
	console.log(err);
}

let imgsize = new Object();
try{
	imgsize = synchttp.get_image_size("http://wmod.66rpg.com/res/loading.gif");
	console.log(imgsize);
}
catch(err)
{
	console.log(err);
}
