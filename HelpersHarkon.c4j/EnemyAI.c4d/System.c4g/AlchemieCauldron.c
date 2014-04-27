/* Objekte, denen die KI ausweichen sollte */

#strict 2

#appendto ACLD

private func Brewing() 
{
  // Noch nicht fertig
  var iBrewTime = 35*2;
	if(product) iBrewTime = product->~BrewTime();
  if (GetActTime() < iBrewTime) return 0;
  // Produkt erzeugen
	var pProduct;
	if(product)
		pProduct = CreateContents(product);
  // Sound
  Sound("Magic1");
  // Aktion beenden
  ProductionCancel();
  // Arbeiter noch da
  if (worker && product)
    // Fasst uns noch an
    if ((worker->GetAction() == "Push") && (worker->GetActionTarget() == this))
      // Arbeiter bekommt den Trank in die Hand
      worker->Collect(pProduct);  
  return 1;
}