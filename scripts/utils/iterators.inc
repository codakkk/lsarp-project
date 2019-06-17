#define _IterN@%0\32; _IterN@
#define _IterL@%0\32; _IterL@

#define ITER_SAFE_REMOVE(%0,%1) for (new _IterN@%0=Iter_Prev(%0,%1),_IterL@%0=1;_IterL@%0;%1=Iter_Contains(%0,%1)?%1:_IterN@%0,--_IterL@%0)