red = 8
green = 6
blue = 7
pushbutton = 2

bcn_red = 1
bcn_green = 0
bcn_blue = 0

connection = nil

keepalive = tmr.create()
beacon = tmr.create()

connected_cb = function(status)
	bcn_green = 1
end

got_ip_cb = function(status)
	bcn_red = 0
	bcn_green = 1
end

gpio.mode(red, gpio.OUTPUT)
gpio.mode(green, gpio.OUTPUT)
gpio.mode(blue, gpio.OUTPUT)

gpio.write(red, gpio.LOW)
gpio.write(blue, gpio.LOW)
gpio.write(green, gpio.LOW)

station_cfg = {}
station_cfg.ssid = "Paradajka"
station_cfg.pwd = "cesnacik@papricka.bangladez"
station_cfg.save = true
station_cfg.auto = true
station_cfg.connected_cb = connected_cb
station_cfg.got_ip_cb = got_ip_cb
wifi.setmode(wifi.STATION)
wifi.setphymode(wifi.PHYMODE_G)
wifi.sta.config(station_cfg)
wifi.sta.sleeptype(wifi.MODEM_SLEEP)

receiver = function(sock, data)
	keepalive:start(true)
	print(data)
end

disconnect = function(sock, err)
	connection = nil
end

local telnet = net.createServer(net.TCP, 30)
telnet:listen(23, function(conn)
	conn:on("receive", receiver)
	conn:on("disconnection", disconnect)
	port, ip = conn:getaddr()
	conn:send("NodeMCU telnet server 0.0\nRunning at " .. ip .. ":" .. port .. "\n")
	connection = conn
end
)

-- gpio4 .. pin 2

gpio.mode(pushbutton, gpio.INPUT)
if gpio.read(pushbutton) == 1 then
	bcn_blue = 1
	uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1, 0)
	uart.on("data", "\n", function(data)
		if connection ~= nil then
			keepalive:start(true)
			uptime = tmr.time()
			connection:send("[" .. tmr.time() .. "] " ..data)
		end
	end, 0)
end

keepalive:alarm(10000, tmr.ALARM_AUTO, function()
	if connection ~= nil then
		connection:send(" \b")   -- keepalive, don't send anything, just refresh idle counter. hopefully
	end
end
)

beacon:alarm(2000, tmr.ALARM_AUTO, function()
	if bcn_red == 1 then
		gpio.write(red, gpio.HIGH)
	else
		gpio.write(red, gpio.LOW)
	end

	if bcn_green == 1 then
		gpio.write(green, gpio.HIGH)
	else
		gpio.write(green, gpio.LOW)
	end

	if bcn_blue == 1 then
		gpio.write(blue, gpio.HIGH)
	else
		gpio.write(blue, gpio.LOW)
	end
	tmr.create():alarm(50, tmr.ALARM_SINGLE, function()
		gpio.write(red, gpio.LOW)
		gpio.write(green, gpio.LOW)
		gpio.write(blue, gpio.LOW)
	end
	)
end
)

