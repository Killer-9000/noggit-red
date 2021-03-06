#ifndef NOGGIT_PIEMENU_HPP
#define NOGGIT_PIEMENU_HPP

/* Declaration */
bool BeginPiePopup( const char* pName, int iMouseButton = 0 );
void EndPiePopup();

bool PieMenuItem( const char* pName, bool bEnabled = true );
bool BeginPieMenu( const char* pName, bool bEnabled = true );
void EndPieMenu();

#endif //NOGGIT_PIEMENU_HPP
