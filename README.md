# C-Samples
This repository contains samples from my private repository for the purpose of portfolio demonstration.

About the code in the repository:

-- RESTFunctionLibrary -- 
This function library that can be used to create various REST API calls at runtime from the Unreal engine.  The functiona library allows you to convert Json objects 
into structs defined in UE4, download and convert base64 strings into images, and perform generic GET and POST requests, (typically used in conjunction with the Json converters to get and post information to and from UE4 structs).

-- APGameModeBase --
This parent class was designed with the intent of implementing on change level events.  The idea being that one object may want to do something, such as perform a REST API call before a player changes levels while the change of the level itself may be imitated by a separate class all together.  To achieve the desired behavior: 
1) Bind any function to be called before the level changes to the "ChangeLevel" delegate with "BindOpenLevelDelegate" template
2) Ensure that the bound function calls "OpenLevelDelegateComplete", which in turn unbinds it from the delegate and opens the level if the delegate has no other bound fucntions
3) When it's time to change level use ensure you use the "OpenLevel" function from the game mode


