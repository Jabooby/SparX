#include <Arduino.h>
#include <librobus.h>

void ChangeOrientation(int orientation);
int main(void)
{
    int y1;
    int y2;
    int x1;
    int x2;
while(y1<10)
{
        if(x1<x2)
        {
            ChangeOrientation(3);
        }
        else 
        {
            if(x1>x2)
            {
                ChangeOrientation(1); 
            }
            else
            {
                ChangeOrientation(2);
            }
        }
    if(coordonnee1 appartient a liste)
    {
        if(coordonnee2 appartient a la liste)
        {
            y2=y1;
            if(x1<2){
                x2=x1+2;
            }
            else 
                x2=x1-1;
        }
    }
    else {
    x1=x2;
    y2=y1+1;
    }


}
}
int currentOrientation=2;
void ChangeOrientation(int orientation)
{
  int deltaPos=orientation- currentOrientation;
  if(deltaPos<0);
  // tourne a gauche : abs(deltaPos) * 90 degres//
  if(deltaPos>0);
  // tourne a droite : abs(delataPos) * 90 degres//
  currentOrientation = orientation;
}
