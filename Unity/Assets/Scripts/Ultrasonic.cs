using UnityEngine;

public class Ultrasonic : MonoBehaviour
{
    public string dir;
    private LayerMask layerMask;
    private float _distance = 0;
    public float Distance => _distance;

    private void Start()
    {
        layerMask = LayerMask.GetMask("Wall");
    }

    void Update()
    {
        RaycastHit hit;
        if (Physics.Raycast(transform.position, transform.forward, out hit, 20f, layerMask))
        {
            _distance = hit.distance;
            Debug.DrawRay(transform.position, transform.forward * hit.distance, Color.red);
            //Debug.LogFormat("{0} : {1}", dir, _distance);
        }
    }
}
