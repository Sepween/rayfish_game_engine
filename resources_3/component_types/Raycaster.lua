Raycaster = {

	OnStart = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")
	end,

	OnUpdate = function(self)
		local result = Physics.Raycast(self.rb:GetPosition(), Vector2(0, 1), 100)
		if result == nil then
			Debug.Log("frame : " .. Application.GetFrame() .. " raycast result : nil")
		else
			Debug.Log("frame : " .. Application.GetFrame() .. " raycast result : " .. result.actor:GetName() .. " with normal (" .. result.normal.x .. "," .. result.normal.y .. ") and is_trigger = " .. tostring(result.is_trigger))
			local new_red_x = Actor.Instantiate("RedX")
			local sr = new_red_x:GetComponent("SpriteRenderer")
			local transform = new_red_x:GetComponent("Transform")
			transform.x = result.point.x
			transform.y = result.point.y
		end
	end
}

