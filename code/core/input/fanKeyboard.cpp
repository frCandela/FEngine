#include "core/input/fanKeyboard.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	Keyboard::Keyboard()
	{
		assert( m_keysPressed.size() == m_keysReleased.size() );
		const uint64_t max = std::numeric_limits<uint64_t>::max();
		for ( int buttonIndex = 0; buttonIndex < m_keysPressed.size(); buttonIndex++ )
		{
			m_keysPressed[ buttonIndex ] = max;
			m_keysReleased[ buttonIndex ] = max;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyDown( const Key _key )
	{
		if ( _key == NONE || ImGui::GetIO().WantCaptureKeyboard )
		{
			return false;
		}
		else
		{
			return glfwGetKey( Input::Get().Window(), _key ) == GLFW_PRESS;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyPressed( const Key _key )
	{
		if ( _key == NONE || ImGui::GetIO().WantCaptureKeyboard )
		{
			return false;
		}
		else
		{
			return Get().m_keysPressed[ _key ] == Input::Get().FrameCount();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyReleased( const Key _key )
	{
		if ( _key == NONE || ImGui::GetIO().WantCaptureKeyboard )
		{
			return false;
		}
		else
		{
			return Get().m_keysReleased[ _key ] == Input::Get().FrameCount();
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	std::string Keyboard::GetKeyName( const Key _key )
	{
		const char* glfwName = glfwGetKeyName( _key, 0 );
		if ( glfwName == nullptr )
		{
			return keyName[ _key ];
		}
		else
		{
			std::string name( glfwName );

			for ( int charIndex = 0; charIndex < name.size(); charIndex++ )
			{
				name[ charIndex ] = ( char ) std::toupper( name[ charIndex ] );
			}
			name.resize( 15, ' ' );

			return name;
		}
	}

	//================================================================================================================================
	// Modifiers are not reliable across systems
	//================================================================================================================================
	void Keyboard::KeyCallback( GLFWwindow* /*_window*/, int _key, int /*_scancode*/, int _action, int /*_mods*/ )
	{
		if ( _key == GLFW_KEY_UNKNOWN ) return;

		ImGuiIO& io = ImGui::GetIO();

		// Dirty hack so that pressing KP_ENTER is considered by IMGUI
		if ( _key == GLFW_KEY_KP_ENTER )
		{
			_key = GLFW_KEY_ENTER;
		}

		//Imgui
		if ( _action == GLFW_PRESS )
			io.KeysDown[ _key ] = true;
		if ( _action == GLFW_RELEASE )
			io.KeysDown[ _key ] = false;

		io.KeyCtrl = io.KeysDown[ GLFW_KEY_LEFT_CONTROL ] || io.KeysDown[ GLFW_KEY_RIGHT_CONTROL ];
		io.KeyShift = io.KeysDown[ GLFW_KEY_LEFT_SHIFT ] || io.KeysDown[ GLFW_KEY_RIGHT_SHIFT ];
		io.KeyAlt = io.KeysDown[ GLFW_KEY_LEFT_ALT ] || io.KeysDown[ GLFW_KEY_RIGHT_ALT ];
		io.KeySuper = io.KeysDown[ GLFW_KEY_LEFT_SUPER ] || io.KeysDown[ GLFW_KEY_RIGHT_SUPER ];

		if ( _action == GLFW_PRESS )
			Get().m_keysPressed[ _key ] = Input::Get().FrameCount();
		else if ( _action == GLFW_RELEASE )
			Get().m_keysReleased[ _key ] = Input::Get().FrameCount();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Keyboard::CharCallback( GLFWwindow* /*_window*/, unsigned int _c )
	{
		ImGuiIO& io = ImGui::GetIO();
		if ( _c > 0 && _c < 0x10000 )
		{
			io.AddInputCharacter( ( unsigned short ) _c );
		}
	}

	const char* Keyboard::keyName[ NONE + 1 ] = {
		"","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",
		 "SPACE          "/*32*/		,"","","","","",""
		,"APOSTROPHE     "/*39*/		,"","","",""
		,"COMMA          "/*44*/
		,"MINUS          "
		,"PERIOD         "
		,"SLASH          "
		,"KEY0           "
		,"KEY1           "
		,"KEY2           "/*50*/
		,"KEY3           "
		,"KEY4           "
		,"KEY5           "
		,"KEY6           "
		,"KEY7           "
		,"KEY8           "
		,"KEY9           "/*57*/,""
		,"SEMICOLON      "/*59*/,""
		,"EQUAL          "/*61*/,"","",""
		,"A              "/*65*/
		,"B              "
		,"C              "
		,"D              "
		,"E              "
		,"F              "/*70*/
		,"G              "
		,"H              "
		,"I              "
		,"J              "
		,"K              "
		,"L              "
		,"M              "
		,"N              "
		,"O              "
		,"P              "/*80*/
		,"Q              "
		,"R              "
		,"S              "
		,"T              "
		,"U              "
		,"V              "
		,"W              "
		,"X              "
		,"Y              "
		,"Z              "/*90*/
		,"LEFT_BRACKET   "
		,"BACKSLASH      "
		,"RIGHT_BRACKET  "/*93*/,"",""
		,"GRAVE_ACCENT   "/*96*/,"","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""
		,"WORLD_1        "/*161*/
		,"WORLD_2        "/*162*/,"","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""
		,"ESCAPE         "/*256*/
		,"ENTER          "
		,"TAB            "
		,"BACKSPACE      "
		,"INSERT         "/*260*/
		,"DELETE         "
		,"RIGHT          "
		,"LEFT           "
		,"DOWN           "
		,"UP             "
		,"PAGE_UP        "
		,"PAGE_DOWN      "
		,"HOME           "
		,"END            "/*269*/,"","","","","","","","","",""
		,"CAPS_LOCK      "/*280*/
		,"SCROLL_LOCK    "
		,"NUM_LOCK       "
		,"PRINT_SCREEN   "
		,"PAUSE          "/*284*/,"","","","",""
		,"F1             "/*290*/
		,"F2             "
		,"F3             "
		,"F4             "
		,"F5             "
		,"F6             "
		,"F7             "
		,"F8             "
		,"F9             "
		,"F10            "
		,"F11            "/*300*/
		,"F12            "
		,"F13            "
		,"F14            "
		,"F15            "
		,"F16            "
		,"F17            "
		,"F18            "
		,"F19            "
		,"F20            "
		,"F21            "/*310*/
		,"F22            "
		,"F23            "
		,"F24            "
		,"F25            "/*314*/,"","","","",""
		,"KP_0           "/*320*/
		,"KP_1           "
		,"KP_2           "
		,"KP_3           "
		,"KP_4           "
		,"KP_5           "
		,"KP_6           "
		,"KP_7           "
		,"KP_8           "
		,"KP_9           "
		,"KP_DECIMAL     "/*330*/
		,"KP_DIVIDE      "
		,"KP_MULTIPLY    "
		,"KP_SUBTRACT    "
		,"KP_ADD         "
		,"KP_ENTER       "
		,"KP_EQUAL       "/*336*/, "", "", ""
		,"LEFT_SHIFT     "/*340*/
		,"LEFT_CONTROL   "
		,"LEFT_ALT       "
		,"LEFT_SUPER     "
		,"RIGHT_SHIFT    "
		,"RIGHT_CONTROL  "
		,"RIGHT_ALT      "
		,"RIGHT_SUPER    "
		,"MENU           "/*348*/
		,"               "/*349*/
	};

	const std::vector<Keyboard::Key> Keyboard::s_keysList =
	{ SPACE
		,APOSTROPHE
		,COMMA
		,MINUS
		,PERIOD
		,SLASH
		,KEY0
		,KEY1
		,KEY2
		,KEY3
		,KEY4
		,KEY5
		,KEY6
		,KEY7
		,KEY8
		,KEY9
		,SEMICOLON
		,EQUAL
		,A
		,B
		,C
		,D
		,E
		,F
		,G
		,H
		,I
		,J
		,K
		,L
		,M
		,N
		,O
		,P
		,Q
		,R
		,S
		,T
		,U
		,V
		,W
		,X
		,Y
		,Z
		,LEFT_BRACKET
		,BACKSLASH
		,RIGHT_BRACKET
		,GRAVE_ACCENT
		,WORLD_1
		,WORLD_2
		,ESCAPE
		,ENTER
		,TAB
		,BACKSPACE
		,INSERT
		,DELETE
		,RIGHT
		,LEFT
		,DOWN
		,UP
		,PAGE_UP
		,PAGE_DOWN
		,HOME
		,END
		,CAPS_LOCK
		,SCROLL_LOCK
		,NUM_LOCK
		,PRINT_SCREEN
		,PAUSE
		,F1
		,F2
		,F3
		,F4
		,F5
		,F6
		,F7
		,F8
		,F9
		,F10
		,F11
		,F12
		,F13
		,F14
		,F15
		,F16
		,F17
		,F18
		,F19
		,F20
		,F21
		,F22
		,F23
		,F24
		,F25
		,KP_0
		,KP_1
		,KP_2
		,KP_3
		,KP_4
		,KP_5
		,KP_6
		,KP_7
		,KP_8
		,KP_9
		,KP_DECIMAL
		,KP_DIVIDE
		,KP_MULTIPLY
		,KP_SUBTRACT
		,KP_ADD
		,KP_ENTER
		,KP_EQUAL
		,LEFT_SHIFT
		,LEFT_CONTROL
		,LEFT_ALT
		,LEFT_SUPER
		,RIGHT_SHIFT
		,RIGHT_CONTROL
		,RIGHT_ALT
		,RIGHT_SUPER
		,MENU
	};
}