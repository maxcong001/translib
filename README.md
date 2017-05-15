# translib
这个项目归Max所有。仅供学习交流，请勿引用到其他项目，如有需求，请联系savagecm@qq.com。

# road map
now this project is in developing.
To do:
## client side
1. client side with a worker queue.
2. new APP client.add callback fun for every client msg. user fruendly API.
## server side
1. server side with ring buffer to hold buffer from libevent per session.
2. make the ring buffer multi-thread safe.
3. while receiving a new message, first put it into ring buffer. Then another thread read the ring buffer and do the logic work.
