using UnityEngine;
using System.Collections;

public class Plot : MonoBehaviour {
    [SerializeField]
    private ParticleSystem ps;
    [SerializeField]
    private float xScale;
    [SerializeField]
    private float yScale;

    private ParticleSystem.EmitParams p;

    void Start()
    {}

    public void addDataPoint(float value, int index, Color32 color)
    {
        Vector3 pos;
        pos.x = index * xScale;
        pos.y = value * yScale;
        pos.z = 0;
        p.position = pos;
        p.startColor = color;
        p.startSize = xScale * 2;
        ps.Emit(p, 1);
    }
}
