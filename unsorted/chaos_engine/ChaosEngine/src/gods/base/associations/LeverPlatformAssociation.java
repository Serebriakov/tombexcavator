package gods.base.associations;

import gods.base.ControlObject;
import gods.base.GfxObject;
import gods.base.LevelData;
import gods.sys.ParameterParser;

public class LeverPlatformAssociation extends ObjectAssociation {

	public LeverPlatformAssociation()
	{
		super(Type.Lever_Platform,2);
	}
	@Override
	public String describe()
	{
		return "if hero pulls \""+get_lever().get_name()+"\" lever then move \""+get_platform().get_name()+"\"";		
		
	}
	@Override
	public void parse(ParameterParser fr, LevelData ld) throws java.io.IOException
	{
		set_object(0,ld.get_bonus(fr.readString("object")));
		set_object(1,ld.get_control_object(fr.readString("object")));
	}

	public GfxObject get_lever()
	{
		return (GfxObject)get_object(0);
	}
	public ControlObject get_platform()
	{
		return (ControlObject)get_object(1);
	}
	
}
