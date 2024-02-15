#include "Camera.hpp"
#include <iostream>
namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {

 /*       printf("+++++++++++++++++++++++++++++++++++\n");
        printf("camera position:: !!!!!!!!!%f %f %f\n", this->cameraPosition.x, this->cameraPosition.y, this->cameraPosition.z);
        printf("camera target:: !!!!!!!!!%f %f %f\n", this->cameraTarget.x, this->cameraTarget.y, this->cameraTarget.z);
        printf("camera up dir:: !!!!!!!!!%f %f %f\n", this->cameraUpDirection.x, this->cameraUpDirection.y, this->cameraUpDirection.z);
        printf("+++++++++++++++++++++++++++++++++++\n");*/

        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, cameraUpDirection));

        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        }

        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {



        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("camera position:: !!!!!!!!!%f %f %f\n", this->cameraPosition.x, this->cameraPosition.y, this->cameraPosition.z);
        printf("camera target:: !!!!!!!!!%f %f %f\n", this->cameraTarget.x, this->cameraTarget.y, this->cameraTarget.z);
        printf("camera up dir:: !!!!!!!!!%f %f %f\n", this->cameraUpDirection.x, this->cameraUpDirection.y, this->cameraUpDirection.z);
        printf("\n\npitch yaw:: !!!!!!!!!% f % f\n", pitch, yaw);
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");


        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::rotate(matrix, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(pitch),
            glm::normalize(glm::cross(cameraUpDirection, cameraTarget - cameraPosition)));

        cameraTarget = cameraPosition + glm::vec3(matrix *
            glm::vec4(cameraTarget - cameraPosition, 1.0f));
    }
}
