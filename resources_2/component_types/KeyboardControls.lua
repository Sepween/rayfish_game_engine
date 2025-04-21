KeyboardControls = {
	speed = 5,
	jump_power = 350,

	OnStart = function(self)
		self.light = self.actor:GetComponent("Light")
		self.directionAngle = 0
	end,

	OnUpdate = function(self)
		-- Angle
		if Input.GetKey("right") then
			self.light.angle = self.light.angle + 3
		end

		if Input.GetKey("left") then
			self.light.angle = self.light.angle - 3
		end

		-- Direction
		if Input.GetKey("up") then
			self.directionAngle = self.directionAngle + 3;
		end

		if Input.GetKey("down") then
			self.directionAngle = self.directionAngle - 3;
		end

		if self.directionAngle >= 360 then
			self.directionAngle = self.directionAngle - 360
		end

		local angleRad = self.directionAngle * (math.pi / 180)

		self.light.direction = Vector2(math.cos(angleRad), math.sin(angleRad))
	end
}

