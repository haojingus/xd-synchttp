'use strict';
const synchttp = require('../src/build/Release/binding');
//let content = "";
try{
	let imgsize = synchttp.get_image_size("http://wmod.66rpg.com/res/loading.gif");

	//content = synchttp.http_get("http://www.baidu.com",10);
	//console.log(content);
	console.log(imgsize);
}
catch(err)
{
	console.log(err);
}


