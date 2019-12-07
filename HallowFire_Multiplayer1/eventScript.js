// change the effect on character - moving platform collision on score updation
// false - Score updation follows normal game logic (collision would end the game and score would not update)
// true - Score updation continues irrespective of the collision, game never ends
function myEvent(a, b) {
	return 'false';
}
