// projekt1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include<iostream>
#include <stack>
using namespace std;
using namespace System;
using namespace System::Drawing;

void histogramRGB( Bitmap^ obraz, double odch, std::string nazwa_obrazu );
void histogramMono( Bitmap^ obraz, double odch, std::string nazwa_obrazu );

void ordFilt2RGB( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu );
void ordFilt2Mono( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu );

void openMono( Bitmap^ obraz, double length, double dgr, std::string nazwa_obrazu );
void openLogic( Bitmap^ obraz, double length, double dgr, std::string nazwa_obrazu );

void holeFillLogic( Bitmap^ obraz, std::string nazwa_obrazu );

struct punkt {
  int x;
  int y;
};
typedef struct punkt punkt;


Bitmap^ floodFill( Bitmap^ obraz, Color old, Color fill )
{
  Color Px;
  int h = obraz->Height;
  int w = obraz->Width;
  punkt start;
  start.x = h / 2;
  start.y = w / 2;
  stack<punkt> stos;
  stos.push( start );
  while ( stos.size() > 0 )
  {
    punkt p = stos.top();
    stos.pop();
    if ( p.y < 0 || p.y > w - 1 || p.x < 0 || p.x > h - 1 )
      continue;
    Color val = obraz->GetPixel( p.y, p.y );
    if ( val.R == old.R )
    {
      obraz->SetPixel( p.y, p.x, fill );
      punkt p0{ p.x + 1, p.y };
      stos.push(p0);
      punkt p1{ p.x - 1, p.y };
      stos.push( p1 );
      punkt p2{ p.x , p.y + 1 };
      stos.push( p2 );
      punkt p3{ p.x, p.y- 1 };
      stos.push( p3 );

    }
  }
  return obraz;
}
int main()
{
    std::cout << "Podaj nazwe obrazu!\n";
    std::string nazwa_obrazu;
    cin >> nazwa_obrazu;
    Bitmap^ obraz;
    String^ newSystemString = gcnew String( nazwa_obrazu.c_str() );
    obraz = gcnew Bitmap( newSystemString, true );
    holeFillLogic( obraz, nazwa_obrazu );
}

void histogramRGB( Bitmap^ obraz, double odch, std::string nazwa_obrazu ) {

}
void histogramMono( Bitmap^ obraz, double odch, std::string nazwa_obrazu )
{
}
void ordFilt2RGB( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu )
{
}
void ordFilt2Mono( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu )
{
}
void openMono( Bitmap^ obraz, double length, double dgr, std::string nazwa_obrazu )
{
}
void openLogic( Bitmap^ obraz, double length, double dgr, std::string nazwa_obrazu )
{
}
void holeFillLogic( Bitmap^ obraz, std::string nazwa_obrazu )
{
  Bitmap^ inverse;
  Bitmap^ output;
  Bitmap^ border;

  int wys, szer;
  int kz, kx;
  wys = obraz->Height;
  szer = obraz->Width;
  inverse = gcnew Bitmap( szer, wys );
  border = gcnew Bitmap( szer, wys );
  output = gcnew Bitmap( szer, wys );
  Color Px;
  for ( kz = 0; kz < wys; kz++ )
  {
    for ( kx = 0; kx < szer; kx++ )
    {
      Px = obraz->GetPixel( kx, kz );
      int pr, pg, pb;
      Px.R == 255 ? pr = 0 : pr = 255;
      Px.G == 255 ? pg = 0 : pg = 255;
      Px.B == 255 ? pb = 0 : pb = 255;
      inverse->SetPixel( kx, kz, Px.FromArgb( pr, pg, pb ) );

      border->SetPixel( 0, kz, Px.FromArgb( 255, 255, 255 ) );
      border->SetPixel( szer-1, kz, Px.FromArgb( 255, 255, 255 ) );
      border->SetPixel( kx, 0, Px.FromArgb( 255, 255, 255 ) );
      border->SetPixel( kx, wys-1, Px.FromArgb( 255, 255, 255 ) );

    }
  }
  while ( 1 )
  {
    for ( kz = 0; kz < wys; kz++ )
    {
      for ( kx = 0; kx < szer; kx++ )
      {
        output->SetPixel( kx, kz, Px.FromArgb( 0, 0, 0 ) );
        if ( inverse->GetPixel( kx, kz ).R )
        {
          if ( border->GetPixel( kx, kz ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kx-1 >=0 && border->GetPixel( kx-1, kz ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kx + 1 < szer && border->GetPixel( kx + 1, kz ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kz - 1 >= 0 && border->GetPixel( kx, kz -1 ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kz + 1  < wys && border->GetPixel( kx, kz +1 ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
        }
      }
    }
    bool same = true;
    for ( kz = 0; kz < wys && same; kz++ )
    {
      for ( kx = 0; kx < szer && same; kx++ )
      {
        if ( border->GetPixel( kx, kz ).R != output->GetPixel( kx, kz ).R )
          same = false;
      }
    }
    if ( same )
      break;
    for ( kz = 0; kz < wys; kz++ )
    {
      for ( kx = 0; kx < szer; kx++ )
      {
        Px = output->GetPixel( kx, kz );
        int pr, pg, pb;
        pr = Px.R;
        pg = Px.G;
        pb = Px.B;
        border->SetPixel( kx, kz, Px.FromArgb( pr, pg, pb ) );
      }
    }
  }
  for ( kz = 0; kz < wys; kz++ )
  {
    for ( kx = 0; kx < szer; kx++ )
    {
      Px = output->GetPixel( kx, kz );
      int pr, pg, pb;
      Px.R == 255 ? pr = 0 : pr = 255;
      Px.G == 255 ? pg = 0 : pg = 255;
      Px.B == 255 ? pb = 0 : pb = 255;
      output->SetPixel( kx, kz, Px.FromArgb( pr, pg, pb ) );
    }
  }
  std:string nowa_nazwa = "filled_" + nazwa_obrazu;
  String^ newSystemString = gcnew String( nowa_nazwa.c_str() );
  output->Save( newSystemString );

}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
