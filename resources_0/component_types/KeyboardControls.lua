KeyboardControls = {
	speed = 0.05,

	OnStart = function(self)
		self.trans = self.actor:GetComponent("Transform")
	end,

	OnUpdate = function(self)
		-- Horizontal
		local horizontal_input = 0
		if Input.GetKey("right") then
			self.trans.x = self.trans.x + self.speed
		end

		if Input.GetKey("left") then
			self.trans.x = self.trans.x - self.speed
		end

		-- Vertical
		local vertical_input = 0
		if Input.GetKey("up") then
			self.trans.y = self.trans.y - self.speed
		end

		if Input.GetKey("down") then
			self.trans.y = self.trans.y + self.speed
		end
	end
}

