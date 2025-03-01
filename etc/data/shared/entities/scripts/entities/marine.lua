script:include( '/data/entities/scripts/entities/character.lua' )

----------------------------------------------------------------
-- Global Variables
----------------------------------------------------------------

Marine = { }

extend( Marine, Character )

----------------------------------------------------------------
-- Local Variables
----------------------------------------------------------------

----------------------------------------------------------------
-- Marine Functions
----------------------------------------------------------------

function Marine.initialize( )

	script:registerEventHandler( 'WEAPON_FIRED', Marine.onEvent )
	script:registerEventHandler( 'WEAPON_STOPPED', Marine.onEvent )
	script:registerEventHandler( 'ACTOR_DEAD', Marine.onEvent )
	script:registerEventHandler( 'ACTOR_HIT', Marine.onEvent )
	
	
	script:playAnimation( 'idle', true )

end

function Marine.onEvent( eventName, var1, var2 )

	if ( var1:getActorName( ) == script:getName( ) ) then
	
		if ( eventName == 'WEAPON_FIRED' ) then
		
			marine:fireWeapon( )
			script:playAnimation( 'hit', true )
			
		end
		
		if ( eventName == 'WEAPON_STOPPED' ) then
		
			marine:stopFiringWeapon( )
			script:playAnimation( 'idle', true )
		
		end
		
		if ( eventName == 'ACTOR_DEAD' ) then
		
			marine:stopFiringWeapon( )
			script:playAnimation( 'die_backward', false )
			
			script:unregisterEventHandler( 'WEAPON_FIRED', Marine.onEvent )
			script:unregisterEventHandler( 'WEAPON_STOPPED', Marine.onEvent )
			script:unregisterEventHandler( 'ACTOR_DEAD', Marine.onEvent )
			script:unregisterEventHandler( 'ACTOR_HIT', Marine.onEvent )
		
		end
		
		if ( eventName == 'ACTOR_HIT' ) then
			print( 'ai hit' )
			marine:onHit( var2 )
		
		end
	
	end

end

marine = Marine:new( )
marine:initialize( )
