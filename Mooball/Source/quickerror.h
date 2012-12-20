///////////////////////////////////////////////////////////////////////////////
//
// QUICKERROR.H
//
// Adapted for the Mooball project 12/09.
// This header defines some routines for error reporting from OpenGL
// Anything that falls under general error reporting can be placed here as well
//
// see: http://googlecode.com/p/mooball
//
/////////////////////////////////////////////////////////////////////////////////



#include "extensions.h"


#ifndef __QUICKERROR_H_
#define __QUICKERROR_H_





// Possible returns from FBO status validation //
enum fbo_status
{
	COMPLETE,
	INCOMPLETE_ATTACHMENT,
	INCOMPLETE_DIMENSIONS,
	MISSING_ATTACHMENT,
	UNSUPPORTED,
	INCOMPLETE_FORMATS,
	INCOMPLETE_DRAW_BUFFER,
	UNKNOWN,
};



// post_gl_errors(void)
// Queries the OpenGL error state and returns the error string if there is one
// Pauses the system until a key is pressed giving the user an opportunity to view the message
static bool	post_gl_errors()
{
	GLenum currentError = glGetError();
	if(currentError != GL_NO_ERROR)
	{
        switch(currentError)
        {
            case GL_INVALID_ENUM:
                printf("OpenGL argument is out of range (GL_INVALID_ENUM)\n");
                break;

            case GL_INVALID_VALUE:
                printf("OpenGL numeric argument is out of range (GL_INVALID_VALUE)\n");
                break;

            case GL_INVALID_OPERATION:
                printf("OpenGL operation is not permitted (GL_INVALID_OPERATION)\n");
                break;

            case GL_STACK_OVERFLOW:
                printf("OpenGL instruction will cause stack overflow (GL_STACK_OVERFLOW)\n");
                break;

            case GL_STACK_UNDERFLOW:
                printf("OpenGL instruction will cause stack underflow (GL_STACK_UNDERFLOW)\n");
                break;

            case GL_OUT_OF_MEMORY:
                printf("OpenGL no memory to execute instruction (GL_OUT_OF_MEMORY)\n");
                break;

			case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
				printf("There was an invalid reference to an operation on an FBO (GL_INVALID_FRAMEBUFFER_OPERATION)\n");
				break;

            default:
				printf("There was an unspecified error.\n");
                break;
        }

		return true;
	}

	return false;   // No error
}


// check_fbo_status(void)
// Returns fbo_status enumeration corresponding to the "completeness" of
// the currently bound framebuffer object (FBO).
//
// COMPLETE- FBO is complete and ready to use
// UNSUPPORTED- normally due to incompatible formats or mismatched sizes
// INCOMPLETE- normally due to missing attachments
static fbo_status check_fbo_status()
{
	GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
	switch(stat)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return COMPLETE;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			return INCOMPLETE_ATTACHMENT;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			return INCOMPLETE_DIMENSIONS;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			return MISSING_ATTACHMENT;

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			return UNSUPPORTED;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			return INCOMPLETE_FORMATS;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			return INCOMPLETE_DRAW_BUFFER;

		default:
			return UNKNOWN;
	}
}



#endif
