
# Throw Specifications



# Special Handler

## Terminate
`terminate()` is called if no exception handler handles a thrown exception. The default implementation
calls just `abort()`

## Unexpected
`unexpected()` is called when a function (or one of the called functions) throws an exception that is not defined in its
throw specification. The default implementation calls `terminate()`.

## Set custom handlers
The functions `terminate()` and `unexpected()` can be customized.

### Terminate Handler
Call:

    terminate_handler set_terminate(terminate_handler newHandler) throw();
	
The function `set_terminate` returns a pointer to the old handler.
	
The `newHandler` function pointer has this type:
    typedef void (*terminate_handler)();
	
***The Terminate Handler needs to exit the program. It's not allowed to return to the program!***


### Unexpected Handler
Call:

    unexpected_handler set_unexpected(unexpected_handler newHandler) throw();
	
The function `set_unexpected` returns a pointer to the old handler.

The `newHandler` function pointer has this type:
    typedef void (*unexpected_handler)();

	
***It's allowed to throw exceptions in this handler. It's also allowed to exit the program. But it's not 
allowed to return to  the program.***


### Customize the `unexpected` Handler

## Uncaught Exception
`uncaught_exception()` is a function that returns *true* if there is a thrown exception that was not (yet) catched.





# Infos
http://stackoverflow.com/a/2820407