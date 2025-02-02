#message:2F:(Online_SearchingForGateway)
{
	Give your shipmate an extra[BR]
	turn to attack.[End]
}


#message:2F:(Online_ConnectingToServer)
{
	[Next]
	Connecting to server...[BR]
	[Yield][End]
}


#message:2F:(Online_Connected)
{
	[Next]
	Connected![BR]
	[Yield][End]
}


#message:2F:(MoveName_RallyWink)
{
	Rally Wink[End]
}


#message:2F:(MoveDesc_RallyWink)
{
	Give your shipmate an extra[BR]
	turn to attack.[End]
}

// TODO
#message:2F:(Personality_Start)
{
[Style right][Voice star]
Let's start the personality[br]
test![br]
[Wait][Yield][End]
}

#message:2F:(Personality_Question_Opportunistic)
{
[Next]
You overhear two merchants[BR]
haggling over a rare item.[Pause 5][BR]
One of them is about to make a[BR]
terrible deal! [Pause 10]What do you do?[BR]
[Yield][End]
}


#message:2F:(Personality_Choice_Opportunistic)
{
[Style Choice pos=45,90 size=235,62]
[DelayOff]
[Cursor 0][Option 0]Jump in with a better offer![BR]
[Cursor 1][Option 1]Convince them it's junk.[BR]
[Cursor 2][Option 2]Ignore them and walk away.[BR]
[DelayOn][EndChoice 3][End]
}


#message:2F:(Personality_Question_Tenacious)
{
[Next]
Your ship has barely survived[br]
days of seige, and it looks like[br]
the enemy is finally retreating.[br][pause 10]
What made them give up?[br]
[Yield][End]
}


#message:2F:(Personality_Choice_Tenacious)
{
[Style Choice pos=60,90 size=210,62]
[DelayOff]
[Cursor 0][Option 0]We scared them off! [NOTE][BR]
[Cursor 1][Option 1]They ran out of supplies.[BR]
[Cursor 2][Option 2]Hmm, it could be a trap...[BR]
[DelayOn][EndChoice 3][End]
}
