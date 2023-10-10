#include <Arduino.h>
#include <LibRobus.h>

int positionsLignes[8][2] = {{2,1},{2,2},{2,1},{3,1},{1,2},{2,2},{1,3},{1,4}};
int x1, y1, x2, y2;
int currentOrientation  = 2; // variable globale de l'orientation
void ChangerOrientation(int orientation)
{
  int localC, localO;
  if (currentOrientation == 1)
    localC = 2;
  else
    localC = 2 * currentOrientation - 3;
  if (orientation == 1)
    localO = 2;
  else
    localO = 2 * orientation - 3;
	int deltaPos = localO - localC;
	if (deltaPos < 0)
		// tourne à gauche : abs(deltaPos) * 90 degrés
	if (deltaPos > 0)
		// tourne à droite : abs(deltaPos) * 90 degrés

	currentOrientation = orientation;
}

void CheckLigne()
{
  bool go = false;
  while (go == false)
  {
    go = true;
    for (int i = 0; i < 8; i++)
    {
      if (positionsLignes[i][0] == x1 && positionsLignes[i][1] == y1)
      {
        if (i%2 == 0)
        {
          if (positionsLignes[i+1][0] == x2 && positionsLignes[i+1][1] == y2)
          {
            ChangerOrientation(currentOrientation + 1);
            x2 = 2 * currentOrientation - 3;
            y2 = y1;
            go = false;
            break;
          }
        }
        else
        {
          if (positionsLignes[i-1][0] == x2 && positionsLignes[i-1][1] == y2)
          {
            ChangerOrientation(currentOrientation + 1);
            x2 = 2 * currentOrientation - 3;
            y2 = y1;
            go = false;
            break;
          }
        }
      }
    }    
  }
}

void setup()
{

}

void loop()
{

}


