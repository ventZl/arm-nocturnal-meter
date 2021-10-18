httpserver = require("httpserver")

httphandler = function(req, res) 
	req.onheader = function(self, name, value)
	end
	if file.exists(req.url) then
		req.ondata = function(self, chunk)
			if not chunk then
				res:send(nil, 200)
				res:send_header("Connection", "close")
				res:send_header("Content-Type", "text/plain")
				f = file.open(req.url)
				res:send(f.read())
				res:finish()
				f.close()
				print("Serving file "..req.url.."...")
			end
		end
	else
		req.ondata = function(self, chunk)
			if not chunk then
				res:send(nil, 200)
				res:send_header("Connection", "close")
				res:send_header("Content-Type", "text/html")
				res:send("<h1>Hello World</h1>")
				res:finish()
				print("Serving static template...");
			end
		end
	end
end

httpserver.createServer(80, httphandler)
