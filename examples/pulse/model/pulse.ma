[top]
components : pulse@pulse
out : out_port
in : start stop
link : start start@pulse
link : stop stop@pulse
link : out@pulse out_port
