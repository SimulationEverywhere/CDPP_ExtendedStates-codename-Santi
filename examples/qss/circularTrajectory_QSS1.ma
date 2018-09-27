[Top]
components : qssX@QSS1 qssY@QSS1 
out : outY outX 
Link : out@qssX in@qssY
Link : out@qssY in@qssX
Link : out@qssY outY
Link : out@qssX outX

[qssX]
dQRel : 0.1
dQMin : 0.1
X0 : 0.5
debug : 1

[qssY]
dQRel : 0.1
dQMin : 0.1
X0 : 4.5
gain : -1


