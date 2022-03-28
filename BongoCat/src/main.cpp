#include "header.hpp"
#include <resource.h>
#define VERSION "Bongo Cat Mver v0.1.1"

sf::RenderWindow window;

#include <windows.h>
//#include <Python.h>

#define BUF_SIZE 2

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


    HANDLE h_pipe;
    char buf_msg[BUF_SIZE];
    DWORD num_rcv; //ʵ�ʽ��յ����ֽ���
    //���������ܵ�,����ΪMyPipe,��Ϣֻ�ܴӿͻ������������,��д���ݲ�������ģʽ,�ֽ�����ʽ,��ʱֵ��Ϊ0��ʾ����Ĭ�ϵ�50���� PIPE_READMODE_MESSAGE INBOUND
    h_pipe = ::CreateNamedPipe("\\\\.\\pipe\\BongoToi", PIPE_ACCESS_INBOUND, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUF_SIZE, BUF_SIZE, 0, nullptr);

    window.create(sf::VideoMode(612, 612), VERSION, sf::Style::Titlebar | sf::Style::Close);

    // get refresh rate and set the frame limit
    DISPLAY_DEVICE device;
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);
    EnumDisplayDevices(NULL, (DWORD)0, &device, 0);
    DEVMODE devmode;
    ZeroMemory(&devmode, sizeof(DEVMODE));
    devmode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings((LPSTR)device.DeviceName, ENUM_REGISTRY_SETTINGS, &devmode);

    // loading configs
    while (!data::init())
        continue;

    bool is_reload = false;

	window.setFramerateLimit(data::cfg["decoration"]["framerateLimit"].asInt());//���֡��
	sf::WindowHandle handle = window.getSystemHandle();
	HICON hIcon;
	hIcon = LoadIcon(hInstance, (LPCSTR)IDI_ICON1);
	if (hIcon) {
		SendMessage(handle, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
	if (data::cfg["decoration"]["topWindow"].asBool())//�ö�����
	{
		SetWindowPos(handle, HWND_TOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE);
	}


    ///////////////////////////////////////////////////////////////////////////////

    HANDLE thread_handle;
    DWORD thread_ID;
    thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcPY, NULL, 0, &thread_ID);

    if (!thread_handle)
        data::error_msg("duoxiancheng sile", "Error loading files");

    //�ȴ������ܵ��ͻ�������
    if (::ConnectNamedPipe(h_pipe, nullptr))
    {
        int duration = data::cfg["memeDuration"].asInt();
        std::string msg = "00";
        while (window.isOpen())
        {    

            sf::Event event;
            while (window.pollEvent(event))
                switch (event.type)
                {
                case sf::Event::Closed:
                    window.close();
                    break;

                // get reload config prompt
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::R && event.key.control)
                    {
                        if (!is_reload)
                            while (!data::init())
                                continue;
                        is_reload = true;
                        break;
                    }
                default:
                    is_reload = false;
                }

            int mode = data::cfg["mode"].asInt();
            int red_value = data::cfg["decoration"]["rgb"][0].asInt();
            int green_value = data::cfg["decoration"]["rgb"][1].asInt();
            int blue_value = data::cfg["decoration"]["rgb"][2].asInt();

            window.clear(sf::Color(red_value, green_value, blue_value));
            morekeys::draw();

            char buf_msg[BUF_SIZE];
            memset(buf_msg, 0, BUF_SIZE);

            std::string msg = "00";
            //��ȡ����
            if (::ReadFile(h_pipe, buf_msg, BUF_SIZE, &num_rcv, nullptr))
            {
                msg = std::string(buf_msg);
                if (std::stoi(msg) != 0)
                {
                    int idx = std::stoi(msg);
                    window.clear(sf::Color(red_value, green_value, blue_value));
                    morekeys::drawDmkReaction(idx);
                }
            }

            window.display();
            if (std::stoi(msg) != 0)
            {
                Sleep(duration);
            }

        }

    }
    ::CloseHandle(h_pipe);
    return 0;
}

// TODO: python�Ѿ��������� ����release��ʧЧ�� ʹ��exe���

DWORD WINAPI ThreadProcPY(LPVOID lpParam) {
    system("liveBilibili.exe");
    while (1) {}

    //int id = data::cfg["liveID"].asInt();
    /////////////////////////////////////////////////////////////////////
    //Py_SetPythonHome((wchar_t*)(L"./PythonEnvs"));
    //Py_Initialize();

    ////PyRun_SimpleString("import sys");
    ////PyRun_SimpleString("sys.path.append('./')");				//��һ������Ҫ���޸�Python·��

    //PyObject* pModule = nullptr;
    //pModule = PyImport_ImportModule("liveBilibili");						//������Ҫ���õ��ļ���py.py,(ע���test.py������python�Դ�����ģ�������ȼ������û���)

    ////pModule = PyImport_Import(PyUnicode_FromString("pTestCPP"));
    ////if (pModule == nullptr)
    ////{
    ////    std::cout << "û�ҵ�" << std::endl;
    ////    return -2;
    ////}

    //PyObject* pyFunc = PyObject_GetAttrString(pModule, "run");		//������Ҫ���õĺ�����
    //PyObject* pyArgs = Py_BuildValue("(i)", id);						//��python����������ֵ
    //PyObject* pyRet = PyObject_CallObject(pyFunc, pyArgs);	//���ú���
    //while (1) {}
    //Py_Finalize();

}
