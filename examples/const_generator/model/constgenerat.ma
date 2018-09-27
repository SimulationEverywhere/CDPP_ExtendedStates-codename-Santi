[top]
components : constgenerator@constgenerator
out : out_port
in : stop
link : stop stop@constgenerator
link : out@constgenerator out_port

[constgenerator]
frequency : 00:00:01:00