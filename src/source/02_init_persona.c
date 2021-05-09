#include <02_init_persona.h>

#include <string.h>  //< strcpy
#include <stdarg.h>  //< va_list, va_start, va_arg

#include <core.h>
#include <utility.h>
#include <game_strings.h>
#include <destiny_structs.h>

static char str_graph[][] =
	{
		{ 97, 32, 32, 32},  // 0
		{ 97, 32, 32, 32},  // 1
		{ 97, 32, 32, 32},  // 2
	  {160, 32, 32, 32},  // 3
		{160, 97, 32, 32},  // 4
		{160,160, 32, 32},  // 5
	  {160,160, 97, 32},  // 6
	  {160,160,160, 32},  // 7
	  {160,160,160, 97},  // 8
		{160,160,160,160}   // 9
	};
	/*
	0,1,2  I              half
	3      II             full
	4      II I           full + half
	5      II II          full + full
	6      II II I        full + full + half
	7      II II II       full + full + full
	8      II II II I     full + full + full + half
	9      II II II II    full + full + full + full
	*/

// Initialize some default values of a PERSONA and add it to the persona vector
void INITIALIZE_PERSONA(Persona_status* persona_stats, char* name, unsigned char num, ...)
{
	va_list valist;
	va_start(valist, num);		
	
	strcpy(persona_stats->name, name);		
	
	persona_stats->land_movement = va_arg(valist, unsigned char);   // we "invert" the value here, so that lower becomes "better" (consumes less energy, avoid doing the computation during each move)
	persona_stats->water_movement = va_arg(valist, unsigned char);  // we "invert" the value here, so that lower becomes "better" (consumes less energy, avoid doing the computation during each move)
	persona_stats->stealth = va_arg(valist, unsigned char);
	
	persona_stats->range = va_arg(valist, unsigned char);
	persona_stats->arrows_max = va_arg(valist, unsigned char);	
	
	persona_stats->att = va_arg(valist, unsigned char);
	persona_stats->def = va_arg(valist, unsigned char);
	
	persona_stats->hp_max = va_arg(valist, unsigned char);
	persona_stats->hp_current = persona_stats->hp_max;
	
	persona_stats->direction = va_arg(valist, unsigned char);	

  va_end(valist);	
	
	++g_pvec_personas_count;	
}

void show_stat(unsigned char x, unsigned char y, const char* description, unsigned char offset, unsigned char value)
{	
	WRITE_STRING(x, y, description, strlen(description));
	
	WRITE_STRING(x+offset, y, str_graph[value], 4);	
}

#define NO_SELECTION 99  // test 1-N, N being number of personas to choose -- 0 is valid, so using 99 as a placeholder as "not yet selected"
void choose_persona()  // Destiny_status* ptr_destiny_stats, vec* pvec_personas)
{    
  Persona_status* ptr_persona_val;
	unsigned char temp_index = NO_SELECTION;  //< temp_index is initially used as the initial_selection
	// ---------------------------	
	unsigned char name_length;	
	
	//static char selectionLEFT[] = {40, 27};
	//static char selectionRIGHT[] = {41, 29};
	
	CLRSCR;
	
	g_pvec_personas_count = 0;
	// FORWARD_YONI_BLACK first
	{	
	  //                                                          arg Lnd Wtr Ste Rng MAX  ATT DEF MAX
	  //                                                            n Mov Mov lth     Arrw          HP  DIRECTION
	  INITIALIZE_PERSONA(&g_pvec_personas[0], str_persona_alcyone,  9,  7,  6,  7,  5,  30,  8,  6,  8, FORWARD_YONI_BLACK);
	  INITIALIZE_PERSONA(&g_pvec_personas[1], str_persona_skadi,    9,  8,  8,  8,  5,  40,  8,  4,  8, FORWARD_YONI_BLACK);
	  INITIALIZE_PERSONA(&g_pvec_personas[2], str_persona_cyrene,   9,  6,  5,  6,  6,  20,  7,  8,  7, FORWARD_YONI_BLACK);
	}
	// BACKWARD_LINGA_WHITE	 list these second
	{
	  INITIALIZE_PERSONA(&g_pvec_personas[3], str_persona_rudra,    9,  7,  6,  6,  7,  80,  9,  7,  6, BACKWARD_LINGA_WHITE);
	  INITIALIZE_PERSONA(&g_pvec_personas[4], str_persona_orion,    9,  8,  9,  7,  4,  30,  8,  7,  8, BACKWARD_LINGA_WHITE);
	  INITIALIZE_PERSONA(&g_pvec_personas[5], str_persona_sidon,    9,  8,  7,  7,  6,  50,  8,  8,  9, BACKWARD_LINGA_WHITE);
	}                                                                                 //^^-- these should be above 20 (minimum of 20 since challenge-X collections are RAND(20))
	
#ifdef QUICK_GAME  
	global_destiny_status.direction = FORWARD_YONI_BLACK;
	global_destiny_status.seed_value = 1234;
	global_destiny_status.blessing_count = MAX_BLESSING_COUNT-1;
	global_destiny_status.persistency_count = MAX_PERSISTENCY_COUNT;
	    
	g_i = 0;

	goto show_stats;
#endif	
	
//reselect:	
	text_banner_center(2, str_choose_your_persona, global_destiny_status.direction);
	
  for (g_i = 0; g_i < g_pvec_personas_count; ++g_i) 
	{
    ptr_persona_val = VEC_GET(g_pvec_personas, g_i);
		
		if (
		  (temp_index == NO_SELECTION)  // auto-select the first compatible direction
		  //&& (ptr_persona_val->direction == global_destiny_status.direction)		  
		)
		{
		  temp_index = g_i;			
		}
				
		name_length = 5+g_i;  //< Re-using name_length for the Y row
		{
			//WRITE_CHAR(10, name_length, 27);  //selectionLEFT[ptr_persona_val->direction]);
			WRITE_1U_DIGIT(11, name_length, (g_i+1) | MASK_HIGH_BIT);
			//WRITE_CHAR(12, name_length, 29);  //selectionRIGHT[ptr_persona_val->direction]);
		}
		
		WRITE_STRING(14, name_length, ptr_persona_val->name, strlen(ptr_persona_val->name));
  }

  WRITE_STRING(0, 22, str_select_persona, STR_SELECT_PERSONA_LEN);
	WRITE_STRING(0, 23, str_press_return_to_proceed, STR_PRESS_RETURN_TO_PROCEED_LEN);
	
	{
		// an initial_selection choice was found -- apply the initial_selection 
		// as the current temp_index, and (queue) to show it -- but also clear out
		// this initial_selection to avoid redundantly showing it
		// (this is a little quirky, but was necessary to support the QUICK_GAME mode
		// since showing the initial_selection initializes certain things)
		//temp_index = initial_selection;
		g_i = temp_index;
		
		goto show_initial_section;
	}
	
	while (TRUE)
	{
	  global_input_ch = GET_PKEY_VIEW;

#ifdef TARGET_C64
    if (global_input_ch == PKEY_NO_KEY)
    {
  		g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
			if (IS_MASK_OFF(g_joy, C64_JOYSTICK_BUTTON))
			{
				global_input_ch = PKEY_RETURN;
			}
			else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_UP))
			{
				global_input_ch = PKEY_W;
			}
			else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_DOWN))
			{
				global_input_ch = PKEY_S;
			}
		}
#endif					
		
	  if (global_input_ch != PKEY_NO_KEY)
	  {			
      if      (global_input_ch == PKEY_1) name_length = 0; 
			else if (global_input_ch == PKEY_2) name_length = 1; 
			else if (global_input_ch == PKEY_3) name_length = 2; 
			else if (global_input_ch == PKEY_4) name_length = 3;
			else if (global_input_ch == PKEY_5) name_length = 4;
			else if (global_input_ch == PKEY_6) name_length = 5;
			else name_length = 255;
			
			if (name_length != 255)
			{
				temp_index = name_length;
			
show_initial_section:			
			
				if (temp_index >= g_pvec_personas_count)
				{
					// BEEP - bad selection
					//DO_BEEP;
				}
				else
				{
					// clear the marker of the now-previous selection
					WRITE_CHAR(8, 5+g_i, ' ');
					
					// move to the next index
					g_i = temp_index;
					
					// show a marker for this new index
					WRITE_CHAR(8, 5+g_i, '>');
				
#ifdef QUICK_GAME
show_stats:				
#endif
          // lookup this persona so we can show status/statistics about it...
			    ptr_persona_val = VEC_GET(g_pvec_personas, g_i);			
			
/*
1234567890123456789012345678901234567890
     LAND MOVE   |||   RANGE    ||
     WATER MOVE  ||||  ATTACK   |||
     STEALTH     ||    DEFENSE  ||
     MAX. ARROWS |     HP MAX   |||

*/			
					show_stat    ( 3, 16, str_stat_land_move,  12, ptr_persona_val->land_movement);
					show_stat    ( 3, 17, str_stat_water_move, 12, ptr_persona_val->water_movement);
					show_stat    ( 3, 18, str_stat_stealth,    12, ptr_persona_val->stealth);
					show_stat    ( 3, 19, str_stat_max_arrows, 12, ptr_persona_val->arrows_max / 10);  // div10 is to scale the arrows_max from 0 to 9
					
					show_stat    (22, 16, str_stat_range,      9, ptr_persona_val->range);
					show_stat    (22, 17, str_stat_attack,     9, ptr_persona_val->att);
					show_stat    (22, 18, str_stat_defense,    9, ptr_persona_val->def);
					show_stat    (22, 19, str_stat_hp_max,     9, ptr_persona_val->hp_max);						
#ifdef QUICK_GAME
          goto final_adjustments;
#endif				
				}
			}
#ifdef TARGET_C64
			else if (
				(global_input_ch == PKEY_W)				
			)
			{				
				if (g_i == 0)
				{
					temp_index = g_pvec_personas_count-1;
				}
				else
				{
					--temp_index;
				}
				goto show_initial_section;	
			}
			else if (
				(global_input_ch == PKEY_S)				
			)
			{
				++temp_index;
				if (temp_index == g_pvec_personas_count)
				{
					temp_index = 0;
				}
				goto show_initial_section;
			}
#endif
			else if (
				(global_input_ch == PKEY_RETURN)
			)
			{
  			break;  // accept the selection
			}		
	  }		
	}	
	// ===============================================================
		
	// Clear the line about pressing 1-6 to select persona... They've made the selection and that no longer applies.
	/*
	for (g_i = 0; g_i < 39; ++g_i)  // WIDTH_OF_SCREEN-1
	{
		WRITE_CHAR(g_i, 22, ' ');
	}
	
	//print_fancy(0, 23, str_press_return_to_proceed, JIFFIES_SIXTEENTH_SECOND);
	WRITE_STRING(0, 23, str_press_return_to_proceed, STR_PRESS_RETURN_TO_PROCEED_LEN);
	WRITE_CHAR(39, 23, 152);  // X
	
	flush_keyboard_buffer();
	g_enter_result = flush_keyboard_and_wait_for_ENTER();	
	if (g_enter_result == TRUE)
	{
		CLRSCR;
		goto reselect;
	}
	*/
	
#ifdef QUICK_GAME	
final_adjustments:	
#endif
  // for display and user selection, "higher is better".... but for computing
	// movement cost, "lower is better" (movement effects the cost of
	// energy to perform the move -- so a lower multiplier is better)
	// so invert the movement stats for the remainder of the game.
	ptr_persona_val->land_movement = 9 - ptr_persona_val->land_movement;
	ptr_persona_val->water_movement = 9 - ptr_persona_val->water_movement;
	ptr_persona_val->stealth = ((9 - ptr_persona_val->stealth) * 2) - 1;
	ROUND_DIVIDE(ptr_persona_val->def, 2);

  g_ptr_persona_status = ptr_persona_val;
}

