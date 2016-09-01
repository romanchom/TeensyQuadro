using UnityEngine;
using System.Collections;
using System.IO.Ports;
using System.Threading;
using System.Text;
using System.IO;

public class Rotator : MonoBehaviour {
	SerialPort port;
	Thread thread;
	Quaternion rot;
    Vector3 mag;
    Vector3 acc;
	string portName;
	Object l;
	string msg = "";
    string magStr = "";
    string accStr = "";
    string rollStr = "";
    string motor1 = "";
    string motor2 = "";
    string angle = "";
	bool go = true;


	// Use this for initialization
	void Start () {
		l = new Object();

		thread = new Thread(Read);

		thread.Start();
	}
	
	// Update is called once per frame
	void Update () {
		lock (l) {
			transform.localRotation = rot;
            Debug.DrawRay(Vector3.zero, rot * mag * 10, Color.red, 0.2f);
            Debug.DrawRay(Vector3.zero, rot * acc * 10, Color.cyan, 0.2f);
        }
	}

	void Open() {
		port = new SerialPort(SerialPort.GetPortNames()[0]);
		port.Open();
	}

	void Read() {
		while (go) {
            try
            {
                if (port == null || !port.IsOpen)
                {
                    msg = "";
                    Open();
                }
                else
                {
                    string line = port.ReadLine();
                    string[] words = line.Split('\t');
                    float[] fs = new float[50];

                    for (int i = 1; i < words.Length - 1; ++i)
                    {
                        float.TryParse(words[i], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture.NumberFormat, out fs[i - 1]);
                    }
                    lock (l)
                    {
                        if (words[0] == "Quat")
                        {
                            rot.w = fs[0];
                            rot.x = -fs[1];
                            rot.y = -fs[3];
                            rot.z = -fs[2];
                        }
                        else if (words[0] == "Mag")
                        {
                            mag.x = fs[0];
                            mag.y = fs[2];
                            mag.z = fs[1];
                            magStr = line;
                        }
                        else if (words[0] == "Acc")
                        {
                            acc.x = fs[0];
                            acc.y = fs[2];
                            acc.z = fs[1];
                            accStr = line;
                        }else if(words[0] == "Roll")
                        {
                            rollStr = line;
                        }
                        else if(words[0] == "Motor1")
                        {
                            motor1 = line;
                        }
                        else if (words[0] == "Motor3")
                        {
                            motor2 = line;
                        }
                        else if (words[0] == "Angle")
                        {
                            angle = line;
                        }
                        else
                        {
                            msg = line;
                        }
                    }
                }
            }
            catch (System.Exception e)
            {
                msg = e.Message;
            }
		}
	}

	void OnDestroy() {
		go = false;
        thread.Join();
		port.Close();
	}

	void OnGUI() {
        StringBuilder str = new StringBuilder();
        str.AppendLine(msg);
        str.AppendLine(accStr);
        str.AppendLine(magStr);
        str.AppendLine(rollStr);
        str.AppendLine(motor1);
        str.AppendLine(motor2);
        str.AppendLine(angle);
        GUI.TextArea(new Rect(0, 0, 500, 500), str.ToString());
	}
}
