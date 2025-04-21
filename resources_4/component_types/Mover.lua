Mover = {
	speed = 0.05,
	upperBound = -2,
	lowerBound = 2,
	directionY = 1,  -- Renamed from dir to directionY for clarity
	
	OnStart = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")
	end,
	
	OnUpdate = function(self)
		-- Apply automatic vertical movement
		self.rb.y = self.rb.y + (self.speed * self.directionY)
		
		-- Check if we've hit the bounds and need to reverse direction
		if self.rb.y >= self.lowerBound then
			self.rb.y = self.lowerBound  -- Clamp to boundary
			self.directionY = -1  -- Change direction to up
		elseif self.rb.y <= self.upperBound then
			self.rb.y = self.upperBound  -- Clamp to boundary
			self.directionY = 1  -- Change direction to down
		end
	end
}