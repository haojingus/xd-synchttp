let process = require('process');
let os = require('os');

let arch = process.config.variables.target_arch;
let os_type = os.platform();

if (os_type=='win32')
{
    if (arch=='ia32')
        module.exports = require('./vendor/win32-i386/binding');
    else if(arch=='x64')
        module.exports = require('./vendor/win32-amd64/binding');
    else
        console.log('not support this arch!');
}
else if(os_type=='linux')
{
    if (arch=='x64')
        module.exports = require('./vendor/linux-x86-amd64/binding');
    else
        console.log('not support this arch!');
}
else if(os_type=='darwin')
{
    if (arch=='x64')
        module.exports = require('./vendor/darwin-x86-amd64/binding');
    else
        console.log('not support this arch!');
}
else
	console.log('not support this arch!');

