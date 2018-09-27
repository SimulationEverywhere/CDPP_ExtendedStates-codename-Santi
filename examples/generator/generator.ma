[top]
components : generator@generator
out : out_port
in : stop
link : stop stop@generator
link : out@generator out_port

[generator]
distribution : exponential
mean : 3
initial : 1
increment : 5