# XD serial library Programmer's Manual

------
## NAME
    http_get - request url by http GET <sync>

## SYNOPSIS
    string http_get(string url,int timeout);
    
## DESCRIPTION
    The  http_get()  function send a sync request by GET,you can set timeout.if timeout is 0,function will be block until finish this request.

## RETURN VALUE
    These functions return a UTF-8 string.
    
## EXCEPITION
    you can catch a serial error by try...catch..
    eg:timeout,socket error,select error...
    