/* Contains default light colors */

#strict 2

/**
 * Returns the light color of candles.
 * @return The color in dword format.
 */
global func LightColorCandle()
{
	return RGBa(255, 216, 132, 70);
}

/**
 * Returns the light color of the light that is attached to crew members.
 * @return The color in dword format.
 */
global func LightColorClonk()
{
	return RGBa(255, 253, 135, 110);
}
