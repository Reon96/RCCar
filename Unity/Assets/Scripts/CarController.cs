using System;
using Unity.VisualScripting;
using UnityEngine;

public class CarController : MonoBehaviour
{
    public Rigidbody rBody;
    float moveForce = 5.0f;
    float rotateSpeed = 1.0f;

    void Start()
    {
        if(rBody == null) rBody = GetComponent<Rigidbody>();
    }
    public void Controller(char dir)
    {
        switch (dir)
        {
            case 'S':
                Move_Stop();
                break;
            case 'F':
                Move_Forward();
                break;
            case 'B':
                Move_Backward();
                break;
            case 'R':
                Move_Stop();
                Move_Turn(1);
                break;
            case 'L':
                Move_Stop();
                Move_Turn(-1);
                break;
        }
    }

    private void Move_Stop()
    {
        rBody.linearVelocity = Vector3.zero;
        rBody.angularVelocity = Vector3.zero;
    }

    private void Move_Forward()
    {
        rBody.AddForce (transform.forward * moveForce, ForceMode.VelocityChange);
    }

    private void Move_Backward()
    {
        rBody.AddForce (-transform.forward * moveForce, ForceMode.VelocityChange);
    }

    private void Move_Turn(int dir)
    {
        rBody.AddTorque(Vector3.up * dir * rotateSpeed, ForceMode.VelocityChange);
    }
}
