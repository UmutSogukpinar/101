using fan_league.Requests.Users;
using fan_league.Responses.Common;
using fan_league.Responses.Users;
using fan_league.Services.Users;
using Microsoft.AspNetCore.Mvc;

namespace fan_league.Controllers;

[ApiController]
[Route("api/[controller]")]
public class UsersController(IUserService userService) : ControllerBase
{
    private readonly IUserService _userService = userService;

    [HttpGet]
    public async Task<ActionResult<PagedResponse<UserResponse>>> GetAll(
        [FromQuery] GetUsersRequest request,
        CancellationToken cancellationToken)
    {
        return Ok(await _userService.GetAllAsync(request, cancellationToken));
    }

    [HttpGet("{id:guid}")]
    public async Task<ActionResult<UserResponse>> GetById(
        Guid id,
        CancellationToken cancellationToken)
    {
        return Ok(await _userService.GetByIdAsync(id, cancellationToken));
    }

    [HttpPost]
    public async Task<ActionResult<UserResponse>> Create(
        CreateUserRequest request,
        CancellationToken cancellationToken)
    {
        UserResponse user = await _userService.CreateAsync(request, cancellationToken);

        return CreatedAtAction(nameof(GetById), new { id = user.Id }, user);
    }

    [HttpPut("{id:guid}")]
    public async Task<ActionResult<UserResponse>> Update(
        Guid id,
        UpdateUserRequest request,
        CancellationToken cancellationToken)
    {
        return Ok(await _userService.UpdateAsync(id, request, cancellationToken));
    }

    [HttpDelete("{id:guid}")]
    public async Task<IActionResult> Delete(
        Guid id,
        CancellationToken cancellationToken)
    {
        await _userService.DeleteAsync(id, cancellationToken);

        return NoContent();
    }
}
