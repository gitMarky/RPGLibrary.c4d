/* Custom money support for Clonks. @br
Certain clonks, such as merchants, have their own money!
By default a Clonk has as much money as his owner.
*/

#strict 2
#appendto CLNK

/**
 * This is how much money a Clonk has.
 * @return The current amount of money that the Clonk has. Overload this function for a custom money system for this Clonk.
 * @note By default, this function calls
 * {@code
 * return GetWealth(GetOwner());
 * }
 */
public func GetMoney()
{
	return GetWealth(GetOwner());
}

/**
 * Sets the amount of money that the Clonk has.
 * @return The new amount of money. Overload this function for a custom money system for this Clonk.
 * @note By default this function calls
 * {@code
 * return SetWealth(GetOwner(), iAmount);
 * }
 */
public func SetMoney(int iAmount)
{
	return SetWealth(GetOwner(), iAmount);
}

/**
 * Gives money to the Clonk.
 * @return The new amount of money. Overload this function for a custom money system for this Clonk.
 * @note By default this function calls
 * {@code
 * return DoWealth(GetOwner(), iAmount);
 * }
 */
public func DoMoney(int iAmount)
{
	return DoWealth(GetOwner(), iAmount);
}

public func GetMoneyInfinite()
{
	return false;
} // not needed for now

