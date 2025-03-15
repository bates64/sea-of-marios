#message:2F:(Online_SearchingForBridgeFirst)
{
	[Style inspect]
	Searching for bridge...[BR]
	[Yield][End]
}







#message:2F:(Online_SearchingForBridgeNext)
{
	[Next]
	Searching for bridge...[BR]
	[Yield][End]
}







#message:2F:(Online_ConnectingToRoom)
{
	[Next]
	Connecting to room...[BR]
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
[Style Choice pos=35,150 size=250,62]
[DelayOff]
[Cursor 0][Option 0]Jump in with a better offer![BR]
[Cursor 1][Option 1]Convince them it's junk[BR]
[Cursor 2][Option 2]Ignore them and walk away[BR]
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
[Style Choice pos=35,150 size=250,62]
[DelayOff]
[Cursor 0][Option 0]We scared them off! [NOTE][BR]
[Cursor 1][Option 1]They ran out of supplies[BR]
[Cursor 2][Option 2]It could be a trap...[BR]
[DelayOn][EndChoice 3][End]
}






#message:2F:(Personality_End)
{
[Next]
One last question...[br][pause 20]
What's your name?[br]
[Wait][Yield][End]
}







#message:2F:(Personality_Question_Precise)
{
[Next]
Are save states cheating?[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Precise)
{
[Style Choice pos=90,150 size=150,62]
[DelayOff]
[Cursor 0][Option 0]Yes[BR]
[Cursor 1][Option 1]No[BR]
[Cursor 2][Option 2]WHATEVER!!![BR]
[DelayOn][EndChoice 3][End]
}







#message:2F:(Personality_Question_Resilient)
{
[Next]
A terrible storm wrecks part of[br]
the ship, leaving a nasty hole[br]
in the deck![br][pause 10]
What do you do?[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Resilient)
{
[Style Choice pos=35,135 size=250,78]
[DelayOff]
[Cursor 0][Option 0]Patch it quickly and move on[BR]
[Cursor 1][Option 1]Take time to fully repair it[BR]
[Cursor 2][Option 2]Get someone else to fix it[BR]
[Cursor 3][Option 3]Do it later[BR]
[DelayOn][EndChoice 4][End]
}







#message:2F:(Personality_Question_Methodical)
{
[Next]
How is your mental health?[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Methodical)
{
[Style Choice pos=100,155 size=110,46]
[DelayOff]
[Cursor 0][Option 0]Good[BR]
[Cursor 1][Option 1]Amazing[BR]
[DelayOn][EndChoice 2][End]
}







#message:2F:(Personality_Question_Strategic)
{
[Next]
There are strange noises[br]
coming from below deck...[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Strategic)
{
[Style Choice pos=35,145 size=250,65]
[DelayOff]
[Cursor 0][Option 0]Ignore it and hope it stops[BR]
[Cursor 1][Option 1]Think carefully before acting[BR]
[Cursor 2][Option 2]M-M-Monsters!!!![BR]
[DelayOn][EndChoice 3][End]
}







#message:2F:(Personality_Question_Creative)
{
[Next]
Your captain gives you a task[br]
with very strict instructions.[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Creative)
{
[Style Choice pos=35,150 size=250,62]
[DelayOff]
[Cursor 0][Option 0]Stick to them exactly.[BR]
[Cursor 1][Option 1]Make a few tweaks.[BR]
[Cursor 2][Option 2]Do it a totally different way.[BR]
[DelayOn][EndChoice 3][End]
}







#message:2F:(Personality_Question_Greedy)
{
[Next]
Have you ever stolen something?[br][pause 20]
Be honest.[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Greedy)
{
[Style Choice pos=35,150 size=250,62]
[DelayOff]
[Cursor 0][Option 0]Yes[BR]
[Cursor 1][Option 1]Never[BR]
[Cursor 2][Option 2]I felt bad so I gave it back...[BR]
[DelayOn][EndChoice 3][End]
}







#message:2F:(Personality_Question_Mischievous)
{
[Next]
[Shake]A runaway train is headed for[br]
a family of Piranha Plants!
You find a lever that can divert[br]
the train.[pause 10] Do you pull it?[br]
[Yield][End]
}







#message:2F:(Personality_Choice_Mischievous)
{
[Style Choice pos=35,150 size=250,62]
[DelayOff]
[Cursor 0][Option 0]Haha! Very funny![BR]
[Cursor 1][Option 1]Save them as soon as possible[BR]
[Cursor 2][Option 2]Let them game over[BR]
[DelayOn][EndChoice 3][End]
}







#message:2F:(Personality_Result_Opportunistic)
{
[Next]
You're always on the lookout for[br]
an opening, ready to seize[br]
opportunities as they appear.[br]
[wait][next]
You have a knack for spotting[br]
weak spots and making the most[br]
of them, often acting swiftly[br]
before others even notice.[br]
[wait][next]
Whether it's a quick gain, or a[br]
chance to advance, you know how[br]
to capitalize on the moment.[br]
[wait][next]
While others hesitate, you're[br]
already moving![pause 10][br]
You're driven by the belief that[br]
timing [pause 10]is [pause 10]everything.[br]
[wait][next]
An opportunistic person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Tenacious)
{
[Next]
You never back down, no matter[br]
how tough things get.[br]
[wait][next]
Persistence is your hallmark.[br][pause 10]
Obstacles only make you dig in[br]
your heels and push forward with[br]
even more determination.[br]
[wait][next]
Your ability to keep going when[br]
others would falter is your[br]
greatest strength.[br]
[wait][next]
A tenacious person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Impulsive)
{
[Next]
[Wave]Ah, the heat of the moment.
That's what you live for.[br][pause 10]
You're a master of jumping into[br]
action without overthinking.[br]
[wait][next]
There's no waiting around for[br]
the perfect moment.[br][pause 10]
If something sparks your[br]
interest, you go for it![br]
[wait][next]
While your decisions may be fast[br]
and "unpredictable", you bring[br]
a certain explosive energy[br]
to every situation.[br]
[wait][next]
An impulsive person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Precise)
{
[Next]
You navigate the currents of[br]
life with sharp focus, always[br]
hitting your mark with precision.[br]
[wait][next]
You always know exactly where[br]
you're headed and take great[br]
care to make every move count.[br]
[wait][next]
You focus on the details,[br]
ensuring everything is exactly[br]
as it should be.[br]
[wait][next]
A precise person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Resilient)
{
[Next]
You always face life's challenges[br]
head-on and with a brave face.[br]
[wait][next]
Your tough shell is built to[br]
withstand hardships, and[br]
you just don't get as affected[br]
by things as others seem to.[br]
[wait][next]
Setbacks never keep you down[br]
for long. You adapt and find a[br]
way to keep moving forward.[br]
[wait][next]
A resilient person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Methodical)
{
[Next]
You approach life with a steady,[br]
organized pace, always thinking[br]
ahead and planning your moves.[br]
[wait][next]
Every point on the flight path[br]
of your life is planned with[br]
precision, and you execute each[br]
phase perfectly.[br]
[wait][next]
Even if you are knocked off[br]
course, you avoid rushing.[br]
You prefer to act logically[br]
in the face of adversity.[br]
[wait][next]
A methodical person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Strategic)
{
[Next]
Quiet and observing, you always[br]
wait for the perfect moment to[br]
take action from the shadows.[br]
[wait][next]
With a knack for seeing the big[br]
picture, you stay patient and[br]
calculated.[br]
[wait][next]
Timing is everything, but the[br]
time is not always now.[br][pause 10]
You know when to hold back[br]
and calculate your next move.[br]
[wait][next]
A strategic person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Creative)
{
[Next]
You thrive on new ideas and[br]
unexpected solutions, always[br]
finding fresh ways to approach[br]
a challenge.[br]
[wait][next]
You're never afraid to throw[br]
something new into the mix,[br]
whether it's problem-solving or[br]
expressing yourself.[br]
[wait][next]
Others have to remember to[br]
think outside the box.[br]
You're always above it, and[br]
your perspective is everything.[br]
[wait][next]
A creative person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Mischievous)
{
[Next]
You thrive on chaos and energy,[br]
always ready to stir things up[br]
and keep people on their toes.[br][pause 10]
You're naughty, and you know it.[br]
[wait][next]
You feed off the reactions of[br]
others, and you're constantly[br]
looking for the next opportunity[br]
to be very, very silly.[br]
[wait][next]
You know how to have fun by[br]
pushing limits, often with a wink[br]
and a smile, leaving others to[br]
figure out what just happened.[br]
[wait][next]
A mischievous person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_Result_Greedy)
{
[Next]
[PrintRising][Shake]More! More! More!!!!!![br]
[SizeReset]
[wait][next]
That was my impression of you.[br][pause 10]
What do you think?[br]
[wait][next]
You know how to have fun by[br]
pushing limits, often with a wink[br]
and a smile, leaving others to[br]
figure out what just happened.[br]
[wait][next]
A mischievous person like you[br]
would be...[br]
[yield][END]
}







#message:2F:(Personality_CharacterResult_Goomba)
{
[Next]
[PrintRising]A Goomba![br]
[Wait][END]
}







#message:2F:(Personality_CharacterResult_Koopa)
{
[Next]
[PrintRising]A Koopa![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_BobOmb)
{
[Next]
[PrintRising]A Bob-Omb![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_Parakoopa)
{
[Next]
[PrintRising]A Paratroopa![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_Boo)
{
[Next]
[PrintRising]A Boo![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_Lakitu)
{
[Next]
[PrintRising]A Lakitu![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_CheepCheep)
{
[Next]
[PrintRising]A Cheep-Cheep![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_Bandit)
{
[Next]
[PrintRising]A Bandit![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_DryBones)
{
[Next]
[PrintRising]A Dry Bones![br]
[Wait][END]
}






#message:2F:(Personality_CharacterResult_Fuzzy)
{
[Next]
[PrintRising]A Fuzzy![br]
[Wait][END]
}





#message:2F:(Intro_WorldCK_Talk_Zero)
{
[Style Right]
[Size 32,32][Shake]AHHHHHH!!!!!![BR]
[Wait][END]
}






#message:2F:(Intro_WorldCK_Talk_One)
{
[Style Right]
Where are we!?! And... Oh no...[BR]
NOT YOU TWO! Why did it have to[BR]
be you two? My worst men.[BR]
[Wait][END]
}






#message:2F:(Intro_WorldRM_Talk_Zero)
{
[Style Right]
Sir that's hurtful. I thought we[BR]
were your best men at your-[BR]
[Wait][END]
}






#message:2F:(Intro_WorldCK_Talk_Two)
{
[Style Right]
DON'T EVEN THINK ABOUT[BR]
FINISHING THAT SENTENCE![BR]
[Wait][Next]
Sigh... It didn't go well. I don't[BR]
want to be reminded of that day.[BR]
[Wait][END]
}






#message:2F:(Intro_WorldBM_Talk_Zero)
{
[Style Right]
Well that happened.[BR]
[Wait][Next]
Captain as to answer your[BR]
question. We appear to be in a[BR]
[Color 0x1]Alternate [Color 0x5]Timeline[Color 0xA].[BR]
[Wait][Next]
We are currently in [Color 0x1]A[Color 0x5]T [Color 0xA]7. We are[BR]
from [Color 0x1]A[Color 0x5]T [Color 0xA]5. But besides that.[BR]
[Wait][Next]
We are at a ocean with no[BR]
land insight. However we aren't[BR]
stranded. We just have to take[BR]
control of this ship.[BR]
[Wait][END]
}






#message:2F:(Intro_WorldCK_Talk_Three)
{
[Style Right]
ARGHHHH!!![BR]
[Wait][Next]
So you are telling me we did in[BR]
fact fall into a wormhole...[BR]
[Wait][Next]
First that wretched [Color 0x7]Mario [Color 0xA]beats[BR]
me and my men. That drink was[BR]
ineffective against him and that[BR]
robot.[BR]
[Wait][Next]
And now we are as you put it 2[BR]
[Color 0x5]A[Color 0x1]T[Color 0xA]s away from our own... This[BR]
can't get any worse![BR]
[Wait][END]
}






#message:2F:(Intro_WorldRM_Talk_One)
{
[Style Right]
Uh sir. Is that-[BR]
[Wait][END]
}






#message:2F:(Intro_WorldCK_Talk_Four)
{
[Style Right]
[Color 0x7]MARIO[Color 0xA]![BR]
[Wait][Next]
So you followed me huh? Well you-[BR]
[Wait][END]
}






#message:2F:(Intro_WorldBM_Talk_One)
{
[Style Right]
Captain that's not our [Color 0x7]Mario[Color 0xA].[BR]
[Wait][Next]
The wormhole closed as soon as[BR]
we fell in it. It is improbable that[BR]
he could follow us. I don't even[BR]
think he would follow us here.[BR]
[Wait][END]
}






#message:2F:(Intro_WorldCK_Talk_Five)
{
[Style Right]
ENOUGH![BR]
[Wait][Next]
I don't care if this is not the[BR]
same man who beat me![BR]
[Wait][Next]
I'll make you pay [Color 0x7]Mario[Color 0xA]![BR]
[Wait][END]
}






#message:2F:(Intro_BattleBM_Talk_Zero)
{
[Style Right]
This won't do. He could beat us.[BR]
Even though he doesn't have any[BR]
badges. Our [Color 0x7]Mario[Color 0xA] forgot to equip[BR]
his badges for some reason...[BR]
[Wait][Next]
Either way he was able to beat[BR]
us even without badges so I won't[BR]
be underestimating you![BR]
[Wait][Next]
[Pause 5]VALOR UP![BR]
[Wait][END]
}






#message:2F:(Intro_BattleBM_Talk_One)
{
[Style Right]
The probability of us succeeding[BR]
has been increased immensely![BR]
[Wait][END]
}






#message:2F:(Intro_BattleRM_Talk_Zero)
{
[Style Right]
Blue If he smashes your pretty[BR]
face. I will burn his head![BR]
[Wait][Next]
Nobody messes with my beloved[BR]
Blue![BR]
[Wait][END]
}






#message:2F:(Intro_BattleRM_Talk_One)
{
[Style Right]
[Shake]BLUE NO!!![/fx][BR]
[Wait][Next]
That's it! I have had enough of[BR]
this! I will get revenge for Blue![BR]
Even if I have to use-[BR]
[Wait][Next]
[Size 32,32][CenterX 160][Down 5]FIRE BURST![BR]
[Wait][END]
}






#message:2F:(Intro_BattleCK_Talk_Zero)
{
[Style Right]
He really just did that. The both[BR]
of them were useless as always.[BR]
I will destroy them[Pause 5] after I am[BR]
done with you [Color 0x7]MARIO!
[Wait][END]
}



#message:2F:(Intro_BattleCK_Talk_One)
{
[Style Right]
[Pause 5]Why [Pause 5]can't [Pause 10]I WIN!?![BR]
[Wait][END]
}
