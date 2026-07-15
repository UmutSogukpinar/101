using Microsoft.AspNetCore.Diagnostics;
using Microsoft.AspNetCore.Mvc;

namespace fan_league.ExceptionHandler;

public sealed class GlobalExceptionHandler(
    ILogger<GlobalExceptionHandler> logger
) : IExceptionHandler
{
    public async ValueTask<bool> TryHandleAsync(
        HttpContext httpContext,
        Exception exception,
        CancellationToken cancellationToken)
    {
        LogException(exception);

        ProblemDetails problemDetails = CreateProblemDetails(exception);

        problemDetails.Instance = httpContext.Request.Path;
        problemDetails.Extensions["traceId"] =
            httpContext.TraceIdentifier;

        httpContext.Response.StatusCode =
            problemDetails.Status
            ?? StatusCodes.Status500InternalServerError;

        await httpContext.Response.WriteAsJsonAsync(
            problemDetails,
            cancellationToken);

        return true;
    }

    private static ProblemDetails CreateProblemDetails(
        Exception exception)
    {
        return exception switch
        {
            NotFoundException e => new ProblemDetails
            {
                Status = StatusCodes.Status404NotFound,
                Title = "Resource not found",
                Detail = e.Message,

                Extensions =
                {
                    ["resourceName"] = e.ResourceName,
                    ["resourceId"] = e.ResourceId
                }
            },

            _ => new ProblemDetails
            {
                Status = StatusCodes.Status500InternalServerError,
                Title = "Internal server error",
                Detail = "Unexpected error occurred."
            }
        };
    }

    private void LogException(Exception exception)
    {
        switch (exception)
        {
            case NotFoundException e:
                logger.LogWarning(
                    "Resource not found. Resource: {ResourceName}, ID: {ResourceId}",
                    e.ResourceName,
                    e.ResourceId);
                break;

            default:
                logger.LogError(
                    exception,
                    "An unhandled exception occurred.");
                break;
        }
    }
}
