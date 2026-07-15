namespace fan_league.Requests.Users;

public sealed record CreateUserRequest(
    string Username,
    string Email,
    string Password);
